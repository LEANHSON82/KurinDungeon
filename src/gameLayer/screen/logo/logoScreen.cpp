#include <algorithm>
#include <raylib.h>
#include "logoScreen.h"
#include "gameConfig.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"

ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime)
{
    state.elapsed += deltaTime;
    if (state.elapsed >= 3.0f || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return ScreenAction::LogoFinished;
    }

    return ScreenAction::None;
}

void DrawLogoScreen(const LogoScreenState &state, const Assets &assets)
{
    ClearBackground(shadeColor);
    float fadeIn = std::clamp(state.elapsed / 0.65f, 0.0f, 1.0f);
    float fadeOut = std::clamp((3.0f - state.elapsed) / 0.35f, 0.0f, 1.0f);
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
        DrawTextCenter(assets.font, "Kurin", {screenWidth / 2.0f, screenHeight / 2.0f}, 240.0f, Fade(goldColor, alpha));
    }
}
