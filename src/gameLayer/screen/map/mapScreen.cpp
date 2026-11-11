#include <raylib.h>
#include "mapScreen.h"

// Update
ScreenAction UpdateMapScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::CombatChosen;
    }

    return ScreenAction::None;
}

// Draw
void DrawMapScreen()
{
    DrawText("MAP GAME", 20, 20, 40, RED);
}
