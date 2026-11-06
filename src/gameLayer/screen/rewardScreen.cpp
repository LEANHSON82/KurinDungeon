#include <raylib.h>
#include "rewardScreen.h"
ScreenAction UpdateRewardScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::rewardConfirmed;
    }

    return ScreenAction::none;
}

void DrawRewardScreen()
{
    DrawText("REWARD SCREEN", 20, 20, 40, RED);
}