#pragma once
#include <vector>
#include "cards/card.h"
#include "screen/screenAction.h"

// Update
ScreenAction UpdateRewardScreen(const std::vector<CardId> &choices, int &chosenIndex);

// Draw
void DrawRewardScreen(const std::vector<CardId> &choices);
