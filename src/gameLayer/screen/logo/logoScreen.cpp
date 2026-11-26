#include <algorithm>
#include <raylib.h>
#include "logoScreen.h"
#include "gameConfig.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"

const float logoTime = 3.0f;
const float fadeInTime = 0.65f;
const float fadeOutTime = 0.35f;

ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime)
{
    state.elapsed += deltaTime;
    if (state.elapsed >= logoTime || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return ScreenAction::LogoFinished;
    }

    return ScreenAction::None;
}

void DrawLogoScreen(const LogoScreenState &state, const Assets &assets)
{
    float fadeIn = std::clamp(state.elapsed / fadeInTime, 0.0f, 1.0f);
    float fadeOut = std::clamp((logoTime - state.elapsed) / fadeOutTime, 0.0f, 1.0f);
    float alpha = std::min(fadeIn, fadeOut);

    ClearBackground(shadeColor);
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
        DrawTextCenter(assets.titleFont, "KURIN", {screenWidth / 2.0f, screenHeight / 2.0f}, logoText, Fade(goldColor, alpha));
    }
}
