#include <raylib.h>
#include "restScreen.h"

// Update
ScreenAction UpdateRestScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::RestTaken;
    }
    return ScreenAction::None;
}

// Draw
void DrawRestScreen(const PlayerState &player)
{
    DrawText("REST", 20, 20, 40, RED);
    DrawText(TextFormat("HP: %d / %d", player.hp, player.maxHp), 20, 90, 30, DARKGRAY);
    DrawText(TextFormat("[Enter] Rest and heal %d HP", restHeal), 20, 150, 30, BLACK);
}
