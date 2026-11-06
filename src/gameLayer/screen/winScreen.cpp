#include <raylib.h>
#include "winScreen.h"

ScreenAction UpdateWinScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::returnToMenu;
    }
    return ScreenAction::none;
}

void DrawWinScreen()
{
    DrawText("VICTORY", 20, 20, 40, RED);
}