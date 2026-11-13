#pragma once
#include "screen/screenAction.h"
#include "combat/battle.h"

// Selection
struct BattleScreenState
{
    int selectedCard = -1;
    int target = 0;
};

// Update
ScreenAction UpdateBattleScreen(BattleScreenState &state, Battle &battle);

// Draw
void DrawBattleScreen(const BattleScreenState &state, const Battle &battle);
