#pragma once
#include <map>
#include <memory>
#include <string>
#include <raylib.h>
#include "data/Run.h"
#include "engine/Assets.h"
#include "engine/Scene.h"

using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

class GameEngine
{
protected:
    Assets m_assets;
    Run m_run;
    SceneMap m_sceneMap;
    std::string m_currentScene;
    bool m_running = true;

    RenderTexture2D m_canvas{};
    int m_canvasFilter = -1;

    // A scene change waits until the current scene has finished its update.
    struct SceneChange
    {
        std::string name;
        std::shared_ptr<Scene> scene;
        bool endCurrentScene = false;
    };
    std::unique_ptr<SceneChange> m_nextScene;

    void sUserInput();
    void sendAction(const Action &action);
    void applySceneChange();
    Rectangle viewport() const;

public:
    // hiddenWindow is for automated tests that need assets but no visible window.
    explicit GameEngine(bool hiddenWindow = false);
    ~GameEngine();
    GameEngine(const GameEngine &) = delete;
    GameEngine &operator=(const GameEngine &) = delete;

    void run();
    void update();
    void quit() { m_running = false; }

    // scene may be null to return to a scene that is already in the map.
    void changeScene(const std::string &name, std::shared_ptr<Scene> scene = nullptr, bool endCurrentScene = false);
    std::shared_ptr<Scene> currentScene();

    void startRun(std::uint32_t seed);
    Run &currentRun() { return m_run; }
    const Assets &assets() const { return m_assets; }
};
