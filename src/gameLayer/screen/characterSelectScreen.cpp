#include <raylib.h>
#include "characterSelectScreen.h"

ScreenAction UpdateCharacterSelectScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::characterConfirmed;
    }

    return ScreenAction::none;
}

void DrawCharacterSelectScreen()
{
    DrawText("CHOOSE CHARACTER", 20, 20, 40, RED);
}
