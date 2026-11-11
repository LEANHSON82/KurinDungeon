#include <raylib.h>
#include "loseScreen.h"

// Update
ScreenAction UpdateLoseScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::ReturnToMenu;
    }
    return ScreenAction::None;
}

// Draw
void DrawLoseScreen()
{
    DrawText("GAME OVER", 20, 20, 40, RED);
}
