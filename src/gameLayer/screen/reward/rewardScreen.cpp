#include <raylib.h>
#include "rewardScreen.h"

// Update
ScreenAction UpdateRewardScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::RewardConfirmed;
    }

    return ScreenAction::None;
}

// Draw
void DrawRewardScreen()
{
    DrawText("REWARD SCREEN", 20, 20, 40, RED);
}