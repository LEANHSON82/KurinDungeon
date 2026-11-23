#include "engine/GameEngine.h"
#include <algorithm>
#include "engine/GameConfig.h"
#include "scenes/Scene_Logo.h"

GameEngine::GameEngine(bool hiddenWindow)
{
    unsigned int flags = FLAG_WINDOW_RESIZABLE;
    if (hiddenWindow)
    {
        flags |= FLAG_WINDOW_HIDDEN;
    }
    SetConfigFlags(flags);
    InitWindow(screenWidth, screenHeight, "Kurin");
    if (!hiddenWindow)
    {
        MaximizeWindow();
    }
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(framesPerSecond);

    m_canvas = LoadRenderTexture(screenWidth, screenHeight);
    m_assets.loadFromFile(std::string(GetApplicationDirectory()) + "resources/", "assets.txt");

    changeScene("LOGO", std::make_shared<Scene_Logo>(this));
    applySceneChange();
}

GameEngine::~GameEngine()
{
    m_nextScene.reset();
    m_sceneMap.clear();
    m_assets.unload();
    UnloadRenderTexture(m_canvas);
    CloseAudioDevice();
    CloseWindow();
}

void GameEngine::startRun(std::uint32_t seed)
{
    m_run = Run{};
    m_run.seed = seed;
    m_run.rng.seed(seed);
    m_run.deck = GetStartingDeck();
    TraceLog(LOG_INFO, "Run seed: %u", static_cast<unsigned>(seed));
}

void GameEngine::changeScene(const std::string &name, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    m_nextScene = std::make_unique<SceneChange>(SceneChange{name, std::move(scene), endCurrentScene});
}

void GameEngine::applySceneChange()
{
    if (!m_nextScene)
    {
        return;
    }
    std::unique_ptr<SceneChange> change = std::move(m_nextScene);
    // Without a scene the change means "go back to one that is still in the map".
    bool sceneExists = change->scene || m_sceneMap.count(change->name) > 0;
    if (!sceneExists)
    {
        TraceLog(LOG_ERROR, "Cannot change to unknown scene: %s", change->name.c_str());
        return;
    }
    if (change->endCurrentScene)
    {
        m_sceneMap.erase(m_currentScene);
    }
    if (change->scene)
    {
        m_sceneMap[change->name] = change->scene;
    }
    m_currentScene = change->name;
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}

Rectangle GameEngine::viewport() const
{
    float scale = std::min(GetScreenWidth() / static_cast<float>(screenWidth),
                           GetScreenHeight() / static_cast<float>(screenHeight));
    float width = screenWidth * scale;
    float height = screenHeight * scale;
    return {(GetScreenWidth() - width) / 2.0f, (GetScreenHeight() - height) / 2.0f, width, height};
}

// Once a scene has asked for a scene change, the rest of this frame's input is dropped.
// Otherwise two keys pressed in the same frame would both reach the old scene and, for
// example, add two reward cards.
void GameEngine::sendAction(const Action &action)
{
    if (m_nextScene)
    {
        return;
    }
    currentScene()->doAction(action);
}

// Turns raw keyboard and mouse input into Actions for the current scene.
void GameEngine::sUserInput()
{
    std::shared_ptr<Scene> scene = currentScene();
    for (const auto &[key, name] : scene->getActionMap())
    {
        if (IsKeyPressed(key))
        {
            sendAction(Action(name, "START"));
        }
        if (IsKeyReleased(key))
        {
            sendAction(Action(name, "END"));
        }
    }

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        sendAction(Action("LEFT_CLICK", "START", mouse));
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        sendAction(Action("LEFT_CLICK", "END", mouse));
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        sendAction(Action("RIGHT_CLICK", "START", mouse));
    }
    Vector2 delta = GetMouseDelta();
    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        sendAction(Action("MOUSE_MOVE", "START", mouse));
    }
}

void GameEngine::update()
{
    // The game draws on a fixed-size canvas that is scaled to fit the window.
    Rectangle view = viewport();
    float scale = view.width / screenWidth;
    SetMouseOffset(static_cast<int>(-view.x), static_cast<int>(-view.y));
    SetMouseScale(1.0f / scale, 1.0f / scale);
    // Nearest keeps pixel art crisp when enlarging; bilinear avoids shimmer when shrinking.
    int filter = scale >= 1.0f ? TEXTURE_FILTER_POINT : TEXTURE_FILTER_BILINEAR;
    if (filter != m_canvasFilter)
    {
        SetTextureFilter(m_canvas.texture, filter);
        m_canvasFilter = filter;
    }

    sUserInput();
    applySceneChange();

    BeginTextureMode(m_canvas);
    ClearBackground(BLACK);
    currentScene()->update();
    EndTextureMode();
    applySceneChange();

    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle source{0.0f, 0.0f, static_cast<float>(screenWidth), -static_cast<float>(screenHeight)};
    DrawTexturePro(m_canvas.texture, source, view, {0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
}

void GameEngine::run()
{
    while (m_running && !WindowShouldClose())
    {
        update();
    }
}
