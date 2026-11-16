#include "core/systems/battle/battleSetup.h"
#include "core/components/cards.h"
#include "core/components/combat.h"
#include "core/components/map.h"
#include "core/prefabs/prefabs.h"
#include "core/state/battle.h"
#include "core/state/run.h"
#include "core/systems/battle/combatRules.h"
#include "core/systems/battle/deckSystem.h"
#include "core/systems/battle/turnSystem.h"

void SpawnBattle(ecs::Registry &registry)
{
    RunState &run = registry.Resource<RunState>();
    std::vector<EnemyId> enemyIds = registry.Get<Encounter>(run.currentRoom).enemies;

    BattleState &battle = registry.Resource<BattleState>();
    battle = BattleState{};
    battle.active = true;
    battle.rng.seed(static_cast<std::uint32_t>(run.rng()));
    registry.Resource<BattleEvents>() = BattleEvents{};
    registry.Resource<BattleCommands>() = BattleCommands{};

    registry.Emplace<Block>(run.player);
    for (int slot = 0; slot < static_cast<int>(enemyIds.size()); slot++)
    {
        SpawnEnemy(registry, enemyIds[slot], slot);
    }

    int order = 0;
    registry.View<DeckCard>().Each([&](ecs::Entity card, const DeckCard &) {
        registry.Emplace<InZone>(card, Zone::Draw, order++);
    });
    ShuffleDrawPile(registry);

    ChooseEnemyIntents(registry);
    StartPlayerTurn(registry);
    UpdateBattleResult(registry);
}

void CleanupBattle(ecs::Registry &registry)
{
    for (ecs::Entity entity : registry.View<BattleScoped>().Collect())
    {
        registry.Destroy(entity);
    }
    registry.RemoveAll<InZone>();
    registry.RemoveAll<Block>();
    registry.RemoveAll<Dead>();

    registry.Resource<BattleState>().active = false;
    registry.Resource<BattleEvents>() = BattleEvents{};
    registry.Resource<BattleCommands>() = BattleCommands{};
}
