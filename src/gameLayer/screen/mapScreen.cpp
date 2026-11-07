#include <raylib.h>
#include "mapScreen.h"

ScreenAction UpdateMapScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::CombatChosen;
    }

    return ScreenAction::None;
}

void DrawMapScreen()
{
    DrawText("MAP GAME", 20, 20, 40, RED);
}
