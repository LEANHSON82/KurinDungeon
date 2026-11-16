#include <algorithm>
#include <raylib.h>
#include "game/assets/assets.h"
#include "game/gameConfig.h"
#include "game/scenes/scenes.h"
#include "game/state/scene.h"
#include "game/ui/style.h"
#include "game/ui/text.h"

static const float logoDuration = 3.0f;

void UpdateLogoScene(ecs::Registry &registry)
{
    if (registry.Resource<SceneState>().time >= logoDuration || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        RequestScene(registry, Scene::Menu);
    }
}

void DrawLogoScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    float elapsed = registry.Resource<SceneState>().time;

    ClearBackground(shadeColor);
    float fadeIn = std::clamp(elapsed / 0.65f, 0.0f, 1.0f);
    float fadeOut = std::clamp((logoDuration - elapsed) / 0.35f, 0.0f, 1.0f);
    float alpha = std::min(fadeIn, fadeOut);
    if (IsTextureReady(assets.logo))
    {
        float scale = std::min(1000.0f / assets.logo.width, 700.0f / assets.logo.height);
        float width = assets.logo.width * scale;
        float height = assets.logo.height * scale;
        Rectangle source{0.0f, 0.0f, static_cast<float>(assets.logo.width), static_cast<float>(assets.logo.height)};
        Rectangle destination{(screenWidth - width) / 2.0f, (screenHeight - height) / 2.0f, width, height};
        DrawTexturePro(assets.logo, source, destination, {0.0f, 0.0f}, 0.0f, Fade(WHITE, alpha));
    }
    else
    {
        DrawTextCenter(assets.font, "Kurin", {screenWidth / 2.0f, screenHeight / 2.0f}, logoText, Fade(goldColor, alpha));
    }
}
