#include "scenes/battle/Scene_Battle.h"
#include <algorithm>
#include "engine/GameEngine.h"

// Enemy AI tuning
static constexpr int bossAbilityHpPercent = 50;
static constexpr int bossAbilityBlockMultiplier = 2;
static constexpr int lowHpDefendPercent = 30;
// A healthy enemy repeats the same three turns: attack, debuff (if it has one, otherwise
// attack again), buff.
static constexpr int intentCycleTurns = 3;
static constexpr int debuffTurnInCycle = 2; // turns 2, 5, 8...
static constexpr int buffTurnInCycle = 0;   // turns 3, 6, 9...

// Weak hits deal 3/4 damage, Vulnerable targets take 3/2 damage.
static constexpr int weakNumerator = 3;
static constexpr int weakDenominator = 4;
static constexpr int vulnerableNumerator = 3;
static constexpr int vulnerableDenominator = 2;

EntityVec Scene_Battle::getPile(Zone zone)
{
    EntityVec pile;
    for (const std::shared_ptr<Entity> &card : m_entityManager.getEntities("card"))
    {
        if (card->get<CCard>().zone == zone)
        {
            pile.push_back(card);
        }
    }
    std::sort(pile.begin(), pile.end(), [](const std::shared_ptr<Entity> &a, const std::shared_ptr<Entity> &b) {
        return a->get<CCard>().order < b->get<CCard>().order;
    });
    return pile;
}

int Scene_Battle::pileSize(Zone zone)
{
    return static_cast<int>(getPile(zone).size());
}

bool Scene_Battle::isAlive(const std::shared_ptr<Entity> &enemy) const
{
    return enemy && enemy->tag() == "enemy" && enemy->get<CHealth>().hp > 0;
}

void Scene_Battle::shuffleDrawPile()
{
    EntityVec pile = getPile(Zone::Draw);
    std::shuffle(pile.begin(), pile.end(), m_rng);
    for (int i = 0; i < static_cast<int>(pile.size()); i++)
    {
        pile[i]->get<CCard>().order = i;
    }
}

void Scene_Battle::moveToZone(const std::shared_ptr<Entity> &card, Zone zone)
{
    CCard &data = card->get<CCard>();
    data.zone = zone;
    data.order = m_nextOrder++;
}

void Scene_Battle::sDrawCards(int count)
{
    for (int i = 0; i < count; i++)
    {
        if (pileSize(Zone::Hand) >= maxHandSize)
        {
            return;
        }

        EntityVec drawPile = getPile(Zone::Draw);
        if (drawPile.empty())
        {
            EntityVec discardPile = getPile(Zone::Discard);
            if (discardPile.empty())
            {
                return;
            }
            for (const std::shared_ptr<Entity> &card : discardPile)
            {
                card->get<CCard>().zone = Zone::Draw;
            }
            shuffleDrawPile();
            drawPile = getPile(Zone::Draw);
        }
        moveToZone(drawPile.back(), Zone::Hand);
    }
}

void Scene_Battle::discardHand()
{
    for (const std::shared_ptr<Entity> &card : getPile(Zone::Hand))
    {
        moveToZone(card, Zone::Discard);
    }
}

static bool IsBelowPercent(const CHealth &health, int percent)
{
    return health.hp * 100 < health.maxHp * percent;
}

static CIntent ChooseIntent(const std::shared_ptr<Entity> &enemy, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy->get<CEnemy>().id);
    const CHealth &health = enemy->get<CHealth>();

    if (enemy->has<CBossAbility>() && !enemy->get<CBossAbility>().used &&
        IsBelowPercent(health, bossAbilityHpPercent))
    {
        return CIntent(IntentType::Defend, definition.block * bossAbilityBlockMultiplier, true);
    }
    if (IsBelowPercent(health, lowHpDefendPercent))
    {
        return CIntent(IntentType::Defend, definition.block, false);
    }
    int turnInCycle = turn % intentCycleTurns;
    if (turnInCycle == buffTurnInCycle)
    {
        return CIntent(IntentType::Buff, definition.buff, false);
    }
    if (definition.debuffAmount > 0 && turnInCycle == debuffTurnInCycle)
    {
        return CIntent(IntentType::Debuff, definition.debuffAmount, false);
    }
    // The base damage only: Strength, Weak and Vulnerable are applied by modifyDamage.
    return CIntent(IntentType::Attack, definition.damage, false);
}

void Scene_Battle::chooseEnemyIntents()
{
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        if (isAlive(enemy))
        {
            enemy->add<CIntent>(ChooseIntent(enemy, m_turn));
        }
    }
}

void Scene_Battle::startPlayerTurn()
{
    m_energy = startingEnergy;
    m_player->get<CBlock>().value = 0;
    sDrawCards(handSize);
}

bool Scene_Battle::canPlayCard(const std::shared_ptr<Entity> &card, const std::shared_ptr<Entity> &target) const
{
    if (m_result != BattleResult::InProgress || !card || card->tag() != "card" || card->get<CCard>().zone != Zone::Hand)
    {
        return false;
    }
    const Card &data = GetCard(card->get<CCard>().id);
    if (data.target == Target::SingleEnemy && !isAlive(target))
    {
        return false;
    }
    return m_energy >= data.cost;
}

EntityVec Scene_Battle::getTargets(const Card &card, const std::shared_ptr<Entity> &target)
{
    EntityVec targets;
    switch (card.target)
    {
    case Target::Self:
        break;
    case Target::SingleEnemy:
        targets.push_back(target);
        break;
    case Target::AllEnemies:
        for (const std::shared_ptr<Entity> &enemy : getEnemies())
        {
            if (isAlive(enemy))
            {
                targets.push_back(enemy);
            }
        }
        break;
    }
    return targets;
}

// The part of a card effect that lands on the player.
void Scene_Battle::applyEffectToPlayer(const Card &card, const Effect &effect)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        break;
    case EffectType::Block:
        gainBlock(m_player, m_player, effect.value);
        break;
    case EffectType::DrawCards:
        // The played card is still in hand, so a reshuffle never draws it back.
        sDrawCards(effect.value);
        break;
    case EffectType::GainEnergy:
        m_energy += effect.value;
        break;
    case EffectType::LoseHp:
        loseHp(m_player, effect.value);
        break;
    case EffectType::ApplyStatus:
        if (card.target == Target::Self)
        {
            applyStatus(m_player, m_player, effect.status, effect.value);
        }
        break;
    }
}

// The part of a card effect that lands on one of the card's targets.
void Scene_Battle::applyEffectToEnemy(const Effect &effect, const std::shared_ptr<Entity> &enemy)
{
    // An earlier hit of the same card may already have killed this enemy.
    if (!isAlive(enemy))
    {
        return;
    }
    if (effect.type == EffectType::Damage)
    {
        dealDamage(m_player, enemy, effect.value);
    }
    else if (effect.type == EffectType::ApplyStatus)
    {
        applyStatus(m_player, enemy, effect.status, effect.value);
    }
}

bool Scene_Battle::sPlayCard(const std::shared_ptr<Entity> &card, const std::shared_ptr<Entity> &target)
{
    if (!canPlayCard(card, target))
    {
        return false;
    }

    const Card &data = GetCard(card->get<CCard>().id);
    m_energy -= data.cost;

    EntityVec targets = getTargets(data, target);
    for (const Effect &effect : data.effects)
    {
        applyEffectToPlayer(data, effect);
        for (const std::shared_ptr<Entity> &enemy : targets)
        {
            applyEffectToEnemy(effect, enemy);
        }

        // A card stops as soon as the battle is decided, e.g. when its HP cost kills the player.
        updateResult();
        if (m_result != BattleResult::InProgress)
        {
            break;
        }
    }

    moveToZone(card, data.exhaust || data.kind == CardKind::Power ? Zone::Exhaust : Zone::Discard);
    logBattle(std::string("PlayCard ") + data.name);
    return true;
}

void Scene_Battle::performIntent(const std::shared_ptr<Entity> &enemy)
{
    // Block from the enemy's last action lasted through the player's turn and ends now.
    enemy->get<CBlock>().value = 0;
    const CIntent &intent = enemy->get<CIntent>();
    if (intent.bossAction && enemy->has<CBossAbility>())
    {
        enemy->get<CBossAbility>().used = true;
    }

    switch (intent.type)
    {
    case IntentType::Attack:
        dealDamage(enemy, m_player, intent.value);
        break;
    case IntentType::Defend:
        gainBlock(enemy, enemy, intent.value);
        break;
    case IntentType::Buff:
        applyStatus(enemy, enemy, StatusType::Strength, intent.value);
        break;
    case IntentType::Debuff:
        applyStatus(enemy, m_player, GetEnemyDefinition(enemy->get<CEnemy>().id).debuff, intent.value);
        break;
    }
}

void Scene_Battle::sEnemyTurn()
{
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        if (!isAlive(enemy))
        {
            continue;
        }
        performIntent(enemy);

        updateResult();
        if (m_result != BattleResult::InProgress)
        {
            return;
        }
    }
}

void Scene_Battle::sEndTurn()
{
    if (m_result != BattleResult::InProgress)
    {
        return;
    }

    discardHand();
    // Statuses wear off only after the enemies have acted, so the numbers shown during the
    // player's turn are the numbers that happen. Debuffs the enemies put on the player during
    // their turn do not count down yet, so "Weak 2" lasts two full player turns.
    CStatus playerBefore = m_player->get<CStatus>();
    sEnemyTurn();
    if (m_result == BattleResult::InProgress)
    {
        tickStatuses(m_player, playerBefore);
        for (const std::shared_ptr<Entity> &enemy : getEnemies())
        {
            tickStatuses(enemy, enemy->get<CStatus>());
        }
        m_turn++;
        chooseEnemyIntents();
        startPlayerTurn();
    }
    logBattle("EndTurn");
}

void Scene_Battle::pushEvent(BattleEventType type, const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int value,
                             StatusType status)
{
    m_events.push_back({type, source, target, value, status});
}

int Scene_Battle::modifyDamage(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int base) const
{
    int numerator = 1;
    int denominator = 1;
    int damage = base;
    if (source && source->has<CStatus>())
    {
        const CStatus &status = source->get<CStatus>();
        damage += status.strength;
        if (status.weak > 0)
        {
            numerator *= weakNumerator;
            denominator *= weakDenominator;
        }
    }
    if (target && target->has<CStatus>() && target->get<CStatus>().vulnerable > 0)
    {
        numerator *= vulnerableNumerator;
        denominator *= vulnerableDenominator;
    }
    // Add first, multiply after, round down once at the end.
    return std::max(0, damage) * numerator / denominator;
}

void Scene_Battle::applyStatus(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, StatusType type, int amount)
{
    target->get<CStatus>().add(type, amount);
    pushEvent(BattleEventType::Status, source, target, amount, type);
}

// Takes one point off every counting-down status the entity already had at the start of the round.
void Scene_Battle::tickStatuses(const std::shared_ptr<Entity> &entity, const CStatus &hadAtStart)
{
    CStatus &status = entity->get<CStatus>();
    for (int i = 0; i < statusCount; i++)
    {
        StatusType type = static_cast<StatusType>(i);
        if (GetStatusDefinition(type).countsDown && hadAtStart.amount(type) > 0 && status.amount(type) > 0)
        {
            status.add(type, -1);
        }
    }
}

void Scene_Battle::markDeadAtZeroHp(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target)
{
    if (target->get<CHealth>().hp == 0)
    {
        pushEvent(BattleEventType::Death, source, target, 0);
    }
}

void Scene_Battle::dealDamage(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int amount)
{
    CHealth &health = target->get<CHealth>();
    CBlock &block = target->get<CBlock>();
    int hpBefore = health.hp;
    if (hpBefore == 0)
    {
        return;
    }

    amount = modifyDamage(source, target, amount);
    if (amount > 0)
    {
        int blocked = std::min(block.value, amount);
        block.value -= blocked;
        health.hp = std::max(0, health.hp - (amount - blocked));
    }
    pushEvent(BattleEventType::Damage, source, target, hpBefore - health.hp);
    markDeadAtZeroHp(source, target);
}

void Scene_Battle::gainBlock(const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int amount)
{
    target->get<CBlock>().value += amount;
    pushEvent(BattleEventType::Block, source, target, amount);
}

void Scene_Battle::loseHp(const std::shared_ptr<Entity> &target, int amount)
{
    CHealth &health = target->get<CHealth>();
    int hpBefore = health.hp;
    if (hpBefore == 0)
    {
        return;
    }
    health.hp = std::max(0, health.hp - std::max(0, amount));
    pushEvent(BattleEventType::HpLoss, target, target, hpBefore - health.hp);
    markDeadAtZeroHp(target, target);
}

void Scene_Battle::updateResult()
{
    if (m_player->get<CHealth>().hp <= 0)
    {
        m_result = BattleResult::Lost;
        return;
    }
    bool anyAlive = std::any_of(getEnemies().begin(), getEnemies().end(), [this](const std::shared_ptr<Entity> &enemy) { return isAlive(enemy); });
    if (!anyAlive)
    {
        m_result = BattleResult::Won;
    }
}

static const char *ResultName(BattleResult result)
{
    switch (result)
    {
    case BattleResult::InProgress:
        return "InProgress";
    case BattleResult::Won:
        return "Won";
    case BattleResult::Lost:
        return "Lost";
    }
    return "";
}

void Scene_Battle::logBattle(const std::string &label)
{
    const CHealth &health = m_player->get<CHealth>();
    TraceLog(LOG_DEBUG, "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Draw %d | Hand %d | Discard %d | Exhaust %d",
             label.c_str(), m_turn, ResultName(m_result), health.hp, m_player->get<CBlock>().value, m_energy,
             pileSize(Zone::Draw), pileSize(Zone::Hand), pileSize(Zone::Discard), pileSize(Zone::Exhaust));
}
