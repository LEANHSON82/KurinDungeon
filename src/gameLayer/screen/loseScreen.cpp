#include <raylib.h>
#include "loseScreen.h"

ScreenAction UpdateLoseScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::returnToMenu;
    }
    return ScreenAction::none;
}

void DrawLoseScreen()
{
    DrawText("GAME OVER", 20, 20, 40, RED);
}
