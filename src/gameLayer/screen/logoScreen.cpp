#include <raylib.h>
#include "logoScreen.h"

static float logoElapsed = 0.0f;

void ResetLogoScreen()
{
    logoElapsed = 0.0f;
}

ScreenAction UpdateLogoScreen(float deltaTime)
{
    logoElapsed += deltaTime;
    if (logoElapsed >= 3.0f)
    {
        return ScreenAction::LogoFinished;
    }

    return ScreenAction::None;
}

void DrawLogoScreen()
{
    DrawText("KURIN", 20, 20, 40, RED);
}
