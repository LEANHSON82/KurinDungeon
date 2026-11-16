#include "core/systems/battle/turnSystem.h"
#include "core/components/combat.h"
#include "core/state/battle.h"
#include "core/systems/battle/combatRules.h"
#include "core/systems/battle/deckSystem.h"

static Intent ChooseIntent(const Enemy &enemy, const Health &health, const Strength &strength, const BossAbility *boss, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);

    if (boss != nullptr && !boss->used && health.hp * 2 < definition.maxHp)
    {
        return Intent{IntentType::Defend, definition.block * 2, true};
    }

    if (health.hp * 10 < definition.maxHp * 3)
    {
        return Intent{IntentType::Defend, definition.block, false};
    }

    if (turn % 3 == 0)
    {
        return Intent{IntentType::Buff, definition.buff, false};
    }

    return Intent{IntentType::Attack, definition.damage + strength.value, false};
}

void ChooseEnemyIntents(ecs::Registry &registry)
{
    int turn = registry.Resource<BattleState>().turn;
    registry.View<Enemy, Health, Strength, Intent>().Each(
        [&](ecs::Entity entity, const Enemy &enemy, const Health &health, const Strength &strength, Intent &intent) {
            if (!registry.Has<Dead>(entity))
            {
                intent = ChooseIntent(enemy, health, strength, registry.TryGet<BossAbility>(entity), turn);
            }
        });
}

void StartPlayerTurn(ecs::Registry &registry)
{
    registry.Resource<BattleState>().energy = startingEnergy;
    registry.Get<Block>(GetPlayer(registry)).value = 0;
    DrawCards(registry, handSize);
}

static void ExecuteEnemyTurn(ecs::Registry &registry)
{
    ecs::Entity player = GetPlayer(registry);

    for (ecs::Entity enemy : GetEnemiesInOrder(registry))
    {
        if (registry.Has<Dead>(enemy))
        {
            continue;
        }

        registry.Get<Block>(enemy).value = 0;

        Intent intent = registry.Get<Intent>(enemy);
        if (intent.bossAction)
        {
            if (BossAbility *boss = registry.TryGet<BossAbility>(enemy))
            {
                boss->used = true;
            }
        }

        switch (intent.type)
        {
        case IntentType::Attack:
            DealDamage(registry, enemy, player, intent.value);
            break;
        case IntentType::Defend:
            GainBlock(registry, enemy, enemy, intent.value);
            break;
        case IntentType::Buff:
            registry.Get<Strength>(enemy).value += intent.value;
            PushEvent(registry, BattleEventType::Buff, enemy, enemy, intent.value);
            break;
        }

        UpdateBattleResult(registry);
        if (registry.Resource<BattleState>().result != BattleResult::InProgress)
        {
            return;
        }
    }
}

void EndPlayerTurn(ecs::Registry &registry)
{
    BattleState &battle = registry.Resource<BattleState>();
    if (!battle.active || battle.result != BattleResult::InProgress)
    {
        return;
    }

    DiscardHand(registry);
    ExecuteEnemyTurn(registry);

    if (battle.result != BattleResult::InProgress)
    {
        return;
    }

    battle.turn++;
    ChooseEnemyIntents(registry);
    StartPlayerTurn(registry);
}
