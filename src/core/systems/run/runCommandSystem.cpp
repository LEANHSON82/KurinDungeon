#include "core/systems/run/runCommandSystem.h"
#include <algorithm>
#include "core/components/combat.h"
#include "core/components/map.h"
#include "core/log.h"
#include "core/prefabs/prefabs.h"
#include "core/state/battle.h"
#include "core/state/run.h"
#include "core/systems/battle/battleSetup.h"
#include "core/systems/run/mapGenerator.h"
#include "core/systems/run/mapRules.h"

static void Emit(ecs::Registry &registry, RunEventType event)
{
    registry.Resource<RunEvents>().pending.push_back(event);
}

static void StartRun(ecs::Registry &registry, std::uint32_t seed)
{
    registry.Clear();
    registry.Resource<BattleState>() = BattleState{};

    RunState &run = registry.Resource<RunState>();
    run = RunState{};
    run.seed = seed;
    run.rng.seed(seed);
    Log(LogLevel::Info, "Run seed: %u", static_cast<unsigned>(seed));

    run.currentRoom = GenerateMap(registry, seed);
    if (!ValidateMap(registry))
    {
        Log(LogLevel::Warning, "Map check failed for seed %u", static_cast<unsigned>(seed));
    }

    run.player = SpawnPlayer(registry);
    SpawnStartingDeck(registry);
    Emit(registry, RunEventType::RunStarted);
}

static void FinishRoom(ecs::Registry &registry)
{
    registry.Emplace<Cleared>(registry.Resource<RunState>().currentRoom);
    Emit(registry, RunEventType::RoomFinished);
}

static void EnterRoom(ecs::Registry &registry, ecs::Entity room)
{
    RunState &run = registry.Resource<RunState>();
    if (registry.Resource<BattleState>().active || !CanVisit(registry, run.currentRoom, room))
    {
        return;
    }

    switch (registry.Get<Room>(room).type)
    {
    case RoomType::Combat:
    case RoomType::Boss:
        run.currentRoom = room;
        SpawnBattle(registry);
        Emit(registry, RunEventType::BattleStarted);
        break;
    case RoomType::Rest:
        run.currentRoom = room;
        Emit(registry, RunEventType::RestStarted);
        break;
    case RoomType::Start:
        break;
    }
}

static void FinishBattle(ecs::Registry &registry)
{
    const BattleState &battle = registry.Resource<BattleState>();
    if (!battle.active || battle.result == BattleResult::InProgress)
    {
        return;
    }

    BattleResult result = battle.result;
    CleanupBattle(registry);

    RunState &run = registry.Resource<RunState>();
    if (result == BattleResult::Lost)
    {
        Emit(registry, RunEventType::BattleLost);
        return;
    }

    if (registry.Get<Room>(run.currentRoom).type == RoomType::Boss)
    {
        registry.Emplace<Cleared>(run.currentRoom);
        Emit(registry, RunEventType::BossDefeated);
        return;
    }

    run.rewardChoices = RollRewards(run.rng, rewardChoiceCount);
    Emit(registry, RunEventType::RewardOffered);
}

static void TakeReward(ecs::Registry &registry, int choice)
{
    RunState &run = registry.Resource<RunState>();
    if (choice < 0 || choice >= static_cast<int>(run.rewardChoices.size()))
    {
        return;
    }

    SpawnDeckCard(registry, run.rewardChoices[choice]);
    run.rewardChoices.clear();
    FinishRoom(registry);
}

static void TakeRest(ecs::Registry &registry)
{
    RunState &run = registry.Resource<RunState>();
    const Room *room = registry.TryGet<Room>(run.currentRoom);
    if (room == nullptr || room->type != RoomType::Rest || registry.Has<Cleared>(run.currentRoom))
    {
        return;
    }

    Health &health = registry.Get<Health>(run.player);
    health.hp = std::min(health.hp + restHeal, health.maxHp);
    FinishRoom(registry);
}

void RunCommandSystem(ecs::Registry &registry)
{
    std::vector<RunCommand> queue;
    queue.swap(registry.Resource<RunCommands>().queue);

    for (const RunCommand &command : queue)
    {
        switch (command.type)
        {
        case RunCommandType::StartRun:
            StartRun(registry, command.seed);
            break;
        case RunCommandType::EnterRoom:
            EnterRoom(registry, command.room);
            break;
        case RunCommandType::FinishBattle:
            FinishBattle(registry);
            break;
        case RunCommandType::TakeReward:
            TakeReward(registry, command.choice);
            break;
        case RunCommandType::TakeRest:
            TakeRest(registry);
            break;
        }
    }
}
