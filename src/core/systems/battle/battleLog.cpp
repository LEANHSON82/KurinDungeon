#include "core/systems/battle/battleLog.h"
#include "core/components/combat.h"
#include "core/log.h"
#include "core/systems/battle/combatRules.h"

static const char *GetResultName(BattleResult result)
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
    return "Unknown";
}

static const char *GetIntentName(IntentType type)
{
    switch (type)
    {
    case IntentType::Attack:
        return "Attack";
    case IntentType::Defend:
        return "Defend";
    case IntentType::Buff:
        return "Buff";
    }
    return "Unknown";
}

void LogBattle(const ecs::Registry &registry, const char *label)
{
    const BattleState &battle = registry.Resource<BattleState>();
    ecs::Entity player = GetPlayer(registry);
    Log(LogLevel::Info,
             "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Draw %d | Hand %d | Discard %d",
             label, battle.turn, GetResultName(battle.result),
             registry.Get<Health>(player).hp, registry.Get<Block>(player).value, battle.energy,
             static_cast<int>(GetPile(registry, Zone::Draw).size()),
             static_cast<int>(GetPile(registry, Zone::Hand).size()),
             static_cast<int>(GetPile(registry, Zone::Discard).size()));

    for (ecs::Entity entity : GetEnemiesInOrder(registry))
    {
        const Enemy &enemy = registry.Get<Enemy>(entity);
        const Intent &intent = registry.Get<Intent>(entity);
        Log(LogLevel::Info, "    Enemy %d %s | HP %d Block %d Strength %d | Intent %s %d",
                 enemy.slot, GetEnemyDefinition(enemy.id).name, registry.Get<Health>(entity).hp,
                 registry.Get<Block>(entity).value, registry.Get<Strength>(entity).value,
                 GetIntentName(intent.type), intent.value);
    }
}
