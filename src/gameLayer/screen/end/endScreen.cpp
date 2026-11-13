#include <raylib.h>
#include "endScreen.h"

// Update
ScreenAction UpdateEndScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::ReturnToMenu;
    }
    return ScreenAction::None;
}

// Draw
void DrawEndScreen(bool won)
{
    DrawText(won ? "VICTORY" : "GAME OVER", 20, 20, 40, RED);
    DrawText("[Enter] Back to menu", 20, 90, 30, DARKGRAY);
}
