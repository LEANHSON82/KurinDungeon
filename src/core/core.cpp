#include "core/core.h"
#include "core/state/battle.h"
#include "core/state/run.h"

void RegisterCoreState(ecs::Registry &registry)
{
    registry.SetResource<RunState>();
    registry.SetResource<RunCommands>();
    registry.SetResource<RunEvents>();
    registry.SetResource<BattleState>();
    registry.SetResource<BattleCommands>();
    registry.SetResource<BattleEvents>();
}
