#include "game/game.h"
#include <raylib.h>
#include "core/core.h"
#include "core/log.h"
#include "core/systems/battle/battleCommandSystem.h"
#include "core/systems/run/runCommandSystem.h"
#include "game/assets/assets.h"
#include "game/scenes/scenes.h"
#include "game/state/battleView.h"
#include "game/systems/playbackSystem.h"
#include "game/systems/sceneFlowSystem.h"

static void ForwardToTraceLog(LogLevel level, const char *message)
{
    TraceLog(level == LogLevel::Warning ? LOG_WARNING : LOG_INFO, "%s", message);
}

Game::Game()
{
    SetLogSink(ForwardToTraceLog);
    RegisterCoreState(registry);

    LoadAssets(registry.SetResource<Assets>());
    registry.SetResource<SceneState>();
    registry.SetResource<BattleUi>();
    registry.SetResource<Playback>();
}

Game::~Game()
{
    UnloadAssets(registry.Resource<Assets>());
    SetLogSink(nullptr);
}

bool Game::ShouldQuit() const
{
    return registry.Resource<SceneState>().quit;
}

void Game::OnEnter(Scene scene)
{
    switch (scene)
    {
    case Scene::Map:
        LayoutMapScene(registry);
        break;
    case Scene::Battle:
        SetupBattleScene(registry);
        break;
    default:
        break;
    }
}

void Game::OnExit(Scene scene)
{
    if (scene == Scene::Battle)
    {
        TeardownBattleScene(registry);
    }
}

void Game::ApplySceneChange()
{
    SceneState &scene = registry.Resource<SceneState>();
    if (scene.next == scene.current)
    {
        return;
    }

    Scene previous = scene.current;
    scene.current = scene.next;
    scene.time = 0.0f;
    OnExit(previous);
    OnEnter(scene.current);
}

void Game::UpdateScene(float deltaTime)
{
    switch (registry.Resource<SceneState>().current)
    {
    case Scene::Logo:
        UpdateLogoScene(registry);
        break;
    case Scene::Menu:
        UpdateMenuScene(registry);
        break;
    case Scene::CharacterSelect:
        UpdateCharacterSelectScene(registry);
        break;
    case Scene::Map:
        UpdateMapScene(registry);
        break;
    case Scene::Battle:
        PlaybackSystem(registry, deltaTime);
        if (!IsPlayingBack(registry))
        {
            BattleInputSystem(registry);
        }
        break;
    case Scene::Reward:
        UpdateRewardScene(registry);
        break;
    case Scene::Rest:
        UpdateRestScene(registry);
        break;
    case Scene::Victory:
    case Scene::GameOver:
        UpdateEndScene(registry);
        break;
    }
}

void Game::Update(float deltaTime)
{
    registry.Resource<SceneState>().time += deltaTime;

    UpdateScene(deltaTime);

    BattleCommandSystem(registry);
    RunCommandSystem(registry);

    SceneFlowSystem(registry);
    ApplySceneChange();
}

void Game::Draw() const
{
    switch (registry.Resource<SceneState>().current)
    {
    case Scene::Logo:
        DrawLogoScene(registry);
        break;
    case Scene::Menu:
        DrawMenuScene(registry);
        break;
    case Scene::CharacterSelect:
        DrawCharacterSelectScene(registry);
        break;
    case Scene::Map:
        DrawMapScene(registry);
        break;
    case Scene::Battle:
        DrawBattleScene(registry);
        break;
    case Scene::Reward:
        DrawRewardScene(registry);
        break;
    case Scene::Rest:
        DrawRestScene(registry);
        break;
    case Scene::Victory:
    case Scene::GameOver:
        DrawEndScene(registry);
        break;
    }
}
