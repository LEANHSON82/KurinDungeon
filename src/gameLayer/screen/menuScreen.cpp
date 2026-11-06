#include <raylib.h>
#include "menuScreen.h"

ScreenAction UpdateMenuScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::startPressed;
    }
    return ScreenAction::none;
}

void DrawMenuScreen()
{
    DrawText("PLAY", 20, 20, 40, RED);
}
