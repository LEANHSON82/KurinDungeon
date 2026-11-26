#include "battle.h"
#include <algorithm>
#include "cards/cardDatabase.h"
#include "combat/damage.h"
#include "combat/enemies/enemyAi.h"
#include "combat/enemies/enemyDatabase.h"
#include "relics/relicDatabase.h"

Battle::Battle(PlayerState &playerState, const std::vector<EnemyId> &enemyIds, const std::vector<RelicId> &relicIds, std::uint32_t seed)
    : player(playerState), relics(relicIds)
{
    for (EnemyId id : enemyIds)
    {
        enemies.push_back(CreateEnemy(id));
    }

    deck.Init(player.masterDeck, seed);
    ChooseEnemyIntents();
    StartPlayerTurn();
    TriggerRelics(RelicTrigger::BattleStart);
    CheckResult();
}

void Battle::StartPlayerTurn()
{
    energy = startingEnergy;
    playerBlock = 0;
    deck.DrawCards(handSize);
    TriggerRelics(RelicTrigger::TurnStart);
}

void Battle::TriggerRelics(RelicTrigger trigger)
{
    for (RelicId id : relics)
    {
        const Relic &relic = GetRelic(id);
        if (relic.trigger != trigger)
        {
            continue;
        }

        ApplyEffectToPlayer(relic.effect, relic.target);
        if (relic.target == Target::AllEnemies)
        {
            for (int i = 0; i < static_cast<int>(enemies.size()); i++)
            {
                ApplyEffectToEnemy(relic.effect, i);
            }
        }
    }
}

bool Battle::IsEnemyAlive(int index) const
{
    return index >= 0 && index < static_cast<int>(enemies.size()) && enemies[index].hp > 0;
}

const Statuses &Battle::GetStatuses(int actor) const
{
    if (actor == playerActor)
    {
        return playerStatus;
    }
    return enemies[actor].status;
}

int Battle::ModifyDamage(int source, int target, int base) const
{
    return ::ModifyDamage(base, GetStatuses(source), GetStatuses(target));
}

int &Battle::EditHp(int actor)
{
    return actor == playerActor ? player.hp : enemies[actor].hp;
}

int &Battle::EditBlock(int actor)
{
    return actor == playerActor ? playerBlock : enemies[actor].block;
}

Statuses &Battle::EditStatuses(int actor)
{
    return actor == playerActor ? playerStatus : enemies[actor].status;
}

bool Battle::CanPlayCard(int handIndex, int targetIndex) const
{
    if (result != BattleResult::InProgress || !deck.IsValidHandIndex(handIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);
    if (card.target == Target::SingleEnemy && !IsEnemyAlive(targetIndex))
    {
        return false;
    }

    return energy >= card.cost;
}

std::vector<int> Battle::GetTargets(const Card &card, int targetIndex) const
{
    std::vector<int> targets;
    switch (card.target)
    {
    case Target::Self:
        break;
    case Target::SingleEnemy:
        targets.push_back(targetIndex);
        break;
    case Target::AllEnemies:
        for (int i = 0; i < static_cast<int>(enemies.size()); i++)
        {
            if (IsEnemyAlive(i))
            {
                targets.push_back(i);
            }
        }
        break;
    }
    return targets;
}

void Battle::ApplyEffectToPlayer(const Effect &effect, Target target)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        break;
    case EffectType::Block:
        GainBlock(playerActor, playerActor, effect.value);
        break;
    case EffectType::DrawCards:
        deck.DrawCards(effect.value);
        break;
    case EffectType::GainEnergy:
        energy += effect.value;
        break;
    case EffectType::LoseHp:
        LoseHp(playerActor, effect.value);
        break;
    case EffectType::ApplyStatus:
        if (target == Target::Self)
        {
            ApplyStatus(playerActor, playerActor, effect.status, effect.value);
        }
        break;
    case EffectType::Heal:
    case EffectType::GainGold:
        break;
    }
}

void Battle::ApplyEffectToEnemy(const Effect &effect, int enemyIndex)
{
    if (!IsEnemyAlive(enemyIndex))
    {
        return;
    }

    if (effect.type == EffectType::Damage)
    {
        DealDamage(playerActor, enemyIndex, effect.value);
    }
    else if (effect.type == EffectType::ApplyStatus)
    {
        ApplyStatus(playerActor, enemyIndex, effect.status, effect.value);
    }
}

bool Battle::PlayCard(int handIndex, int targetIndex)
{
    if (!CanPlayCard(handIndex, targetIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);
    energy -= card.cost;

    std::vector<int> targets = GetTargets(card, targetIndex);
    for (const Effect &effect : card.effects)
    {
        ApplyEffectToPlayer(effect, card.target);
        for (int enemyIndex : targets)
        {
            ApplyEffectToEnemy(effect, enemyIndex);
        }

        CheckResult();
        if (result != BattleResult::InProgress)
        {
            break;
        }
    }

    if (card.kind == CardKind::Attack && result == BattleResult::InProgress)
    {
        TriggerRelics(RelicTrigger::AttackPlayed);
    }
    deck.RemovePlayedCard(handIndex, card.exhaust || card.kind == CardKind::Power);
    return true;
}

void Battle::EndPlayerTurn()
{
    if (result != BattleResult::InProgress)
    {
        return;
    }

    deck.DiscardHand();
    Statuses playerBefore = playerStatus;
    ExecuteEnemyTurn();

    if (result != BattleResult::InProgress)
    {
        return;
    }

    TickStatuses(playerStatus, playerBefore);
    for (Enemy &enemy : enemies)
    {
        TickStatuses(enemy.status, enemy.status);
    }
    turn++;
    ChooseEnemyIntents();
    StartPlayerTurn();
}

void Battle::ChooseEnemyIntents()
{
    for (Enemy &enemy : enemies)
    {
        if (enemy.hp > 0)
        {
            enemy.intent = ChooseIntent(enemy, turn);
        }
    }
}

void Battle::PerformIntent(int enemyIndex)
{
    Enemy &enemy = enemies[enemyIndex];
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);
    enemy.block = 0;

    if (enemy.intent.bossAction && definition.bossAbility)
    {
        enemy.usedBossAction = true;
    }

    switch (enemy.intent.type)
    {
    case IntentType::Attack:
        DealDamage(enemyIndex, playerActor, enemy.intent.value);
        break;
    case IntentType::Defend:
        GainBlock(enemyIndex, enemyIndex, enemy.intent.value);
        break;
    case IntentType::Buff:
        ApplyStatus(enemyIndex, enemyIndex, StatusType::Strength, enemy.intent.value);
        break;
    case IntentType::Debuff:
        ApplyStatus(enemyIndex, playerActor, definition.debuff, enemy.intent.value);
        break;
    }
}

void Battle::ExecuteEnemyTurn()
{
    for (int i = 0; i < static_cast<int>(enemies.size()); i++)
    {
        if (!IsEnemyAlive(i))
        {
            continue;
        }

        PerformIntent(i);

        CheckResult();
        if (result != BattleResult::InProgress)
        {
            return;
        }
    }
}

void Battle::TickStatuses(Statuses &statuses, const Statuses &hadAtStart)
{
    for (int i = 0; i < statusCount; i++)
    {
        StatusType type = static_cast<StatusType>(i);
        if (GetStatusDefinition(type).countsDown && GetStatus(hadAtStart, type) > 0 && GetStatus(statuses, type) > 0)
        {
            AddStatus(statuses, type, -1);
        }
    }
}

void Battle::DealDamage(int source, int target, int amount)
{
    int &hp = EditHp(target);
    int hpBefore = hp;
    if (hpBefore == 0)
    {
        return;
    }

    ApplyDamage(ModifyDamage(source, target, amount), hp, EditBlock(target));
    AddEvent(BattleEventType::Damage, source, target, hpBefore - hp);
    if (hp == 0)
    {
        AddEvent(BattleEventType::Death, source, target, 0);
    }
}

void Battle::GainBlock(int source, int target, int amount)
{
    EditBlock(target) += amount;
    AddEvent(BattleEventType::Block, source, target, amount);
}

void Battle::LoseHp(int target, int amount)
{
    int &hp = EditHp(target);
    int hpBefore = hp;
    if (hpBefore == 0)
    {
        return;
    }

    hp = std::max(0, hp - std::max(0, amount));
    AddEvent(BattleEventType::HpLoss, target, target, hpBefore - hp);
    if (hp == 0)
    {
        AddEvent(BattleEventType::Death, target, target, 0);
    }
}

void Battle::ApplyStatus(int source, int target, StatusType type, int amount)
{
    AddStatus(EditStatuses(target), type, amount);
    AddEvent(BattleEventType::Status, source, target, amount, type);
}

void Battle::CheckResult()
{
    if (player.hp <= 0)
    {
        result = BattleResult::Lost;
        return;
    }

    for (const Enemy &enemy : enemies)
    {
        if (enemy.hp > 0)
        {
            return;
        }
    }
    result = BattleResult::Won;
}

void Battle::AddEvent(BattleEventType type, int source, int target, int value, StatusType status)
{
    events.push_back({type, source, target, value, status});
}

std::vector<BattleEvent> Battle::TakeEvents()
{
    std::vector<BattleEvent> taken;
    taken.swap(events);
    return taken;
}
