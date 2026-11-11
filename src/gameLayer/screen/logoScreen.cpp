#include <raylib.h>
#include "logoScreen.h"

ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime)
{
    state.elapsed += deltaTime;
    if (state.elapsed >= 3.0f)
    {
        return ScreenAction::LogoFinished;
    }

    return ScreenAction::None;
}

void DrawLogoScreen()
{
    DrawText("KURIN", 20, 20, 40, RED);
}
