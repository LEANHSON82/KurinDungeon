#pragma once
#include "screen/screenAction.h"
#include "combat/battle.h"

// Update
ScreenAction UpdateBattleScreen(Battle &battle);

// Draw
void DrawBattleScreen(const Battle &battle);
