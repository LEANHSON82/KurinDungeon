#include "game/systems/sceneFlowSystem.h"
#include "core/state/run.h"
#include "game/state/scene.h"

static Scene GetSceneFor(RunEventType event)
{
    switch (event)
    {
    case RunEventType::RunStarted:
    case RunEventType::RoomFinished:
        return Scene::Map;
    case RunEventType::BattleStarted:
        return Scene::Battle;
    case RunEventType::RestStarted:
        return Scene::Rest;
    case RunEventType::RewardOffered:
        return Scene::Reward;
    case RunEventType::BattleLost:
        return Scene::GameOver;
    case RunEventType::BossDefeated:
        return Scene::Victory;
    }
    return Scene::Map;
}

void SceneFlowSystem(ecs::Registry &registry)
{
    std::vector<RunEventType> events;
    events.swap(registry.Resource<RunEvents>().pending);
    for (RunEventType event : events)
    {
        RequestScene(registry, GetSceneFor(event));
    }
}
