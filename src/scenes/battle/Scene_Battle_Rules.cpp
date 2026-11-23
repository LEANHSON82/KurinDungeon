#include "scenes/battle/Scene_Battle.h"
#include <algorithm>
#include "engine/GameEngine.h"

// Enemy AI tuning
static constexpr int bossAbilityHpPercent = 50;
static constexpr int bossAbilityBlockMultiplier = 2;
static constexpr int lowHpDefendPercent = 30;
static constexpr int buffEveryTurns = 3;

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
        if (static_cast<int>(getPile(Zone::Hand).size()) >= maxHandSize)
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

static bool IsBelowPercent(const CHealth &health, int maxHp, int percent)
{
    return health.hp * 100 < maxHp * percent;
}

static CIntent ChooseIntent(const std::shared_ptr<Entity> &enemy, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy->get<CEnemy>().id);
    const CHealth &health = enemy->get<CHealth>();

    if (enemy->has<CBossAbility>() && !enemy->get<CBossAbility>().used &&
        IsBelowPercent(health, definition.maxHp, bossAbilityHpPercent))
    {
        return CIntent(IntentType::Defend, definition.block * bossAbilityBlockMultiplier, true);
    }
    if (IsBelowPercent(health, definition.maxHp, lowHpDefendPercent))
    {
        return CIntent(IntentType::Defend, definition.block, false);
    }
    if (turn % buffEveryTurns == 0)
    {
        return CIntent(IntentType::Buff, definition.buff, false);
    }
    return CIntent(IntentType::Attack, definition.damage + enemy->get<CStrength>().value, false);
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
        switch (effect.type)
        {
        case EffectType::Damage:
            for (const std::shared_ptr<Entity> &enemy : targets)
            {
                if (isAlive(enemy))
                {
                    dealDamage(m_player, enemy, effect.value);
                }
            }
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
        }

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

void Scene_Battle::sEnemyTurn()
{
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        if (!isAlive(enemy))
        {
            continue;
        }

        enemy->get<CBlock>().value = 0;
        CIntent intent = enemy->get<CIntent>();
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
            enemy->get<CStrength>().value += intent.value;
            pushEvent(BattleEventType::Buff, enemy, enemy, intent.value);
            break;
        }

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
    sEnemyTurn();
    if (m_result == BattleResult::InProgress)
    {
        m_turn++;
        chooseEnemyIntents();
        startPlayerTurn();
    }
    logBattle("EndTurn");
}

void Scene_Battle::pushEvent(BattleEventType type, const std::shared_ptr<Entity> &source, const std::shared_ptr<Entity> &target, int value)
{
    m_events.push_back({type, source, target, value});
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
             static_cast<int>(getPile(Zone::Draw).size()), static_cast<int>(getPile(Zone::Hand).size()),
             static_cast<int>(getPile(Zone::Discard).size()), static_cast<int>(getPile(Zone::Exhaust).size()));
}
