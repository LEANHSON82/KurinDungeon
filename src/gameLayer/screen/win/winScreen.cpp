#include <raylib.h>
#include "winScreen.h"

// Update
ScreenAction UpdateWinScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::ReturnToMenu;
    }
    return ScreenAction::None;
}

// Draw
void DrawWinScreen()
{
    DrawText("VICTORY", 20, 20, 40, RED);
}