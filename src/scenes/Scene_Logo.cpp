#include "scenes/Scene_Logo.h"
#include <algorithm>
#include <memory>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_Menu.h"
#include "ui/Style.h"
#include "ui/Text.h"

static constexpr int logoFrames = 3 * framesPerSecond;
static constexpr float fadeInFrames = 39.0f;
static constexpr float fadeOutFrames = 21.0f;

Scene_Logo::Scene_Logo(GameEngine *game) : Scene(game)
{
    registerAction(KEY_ENTER, "SKIP");
}

void Scene_Logo::update()
{
    if (m_currentFrame >= logoFrames)
    {
        m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
    }
    sRender();
    m_currentFrame++;
}

void Scene_Logo::sDoAction(const Action &action)
{
    bool skip = action.name() == "SKIP" || action.name() == "LEFT_CLICK";
    if (skip && action.type() == "START")
    {
        m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
    }
}

void Scene_Logo::sRender()
{
    const Assets &assets = m_game->assets();
    float frame = static_cast<float>(m_currentFrame);
    float alpha = std::min(std::clamp(frame / fadeInFrames, 0.0f, 1.0f), std::clamp((logoFrames - frame) / fadeOutFrames, 0.0f, 1.0f));

    ClearBackground(shadeColor);
    Texture2D logo = assets.getTexture("Logo");
    if (IsTextureReady(logo))
    {
        float scale = std::min(1000.0f / logo.width, 700.0f / logo.height);
        float width = logo.width * scale;
        float height = logo.height * scale;
        Rectangle source{0.0f, 0.0f, static_cast<float>(logo.width), static_cast<float>(logo.height)};
        Rectangle destination{(screenWidth - width) / 2.0f, (screenHeight - height) / 2.0f, width, height};
        DrawTexturePro(logo, source, destination, {0.0f, 0.0f}, 0.0f, Fade(WHITE, alpha));
    }
    else
    {
        DrawTextCenter(assets.getFont("Main"), "Kurin", {screenWidth / 2.0f, screenHeight / 2.0f}, logoText, Fade(goldColor, alpha));
    }
}
