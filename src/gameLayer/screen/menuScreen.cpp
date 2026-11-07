#include <raylib.h>
#include "menuScreen.h"

ScreenAction UpdateMenuScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::StartPressed;
    }
    return ScreenAction::None;
}

void DrawMenuScreen()
{
    DrawText("PLAY", 20, 20, 40, RED);
}
