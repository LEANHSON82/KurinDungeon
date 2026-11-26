#pragma once
#include <vector>
#include "assets/assets.h"
#include "cards/card.h"
#include "screen/screenAction.h"

ScreenAction UpdateRewardScreen(const std::vector<CardId> &choices, int &chosenIndex);

void DrawRewardScreen(const std::vector<CardId> &choices, const Assets &assets);
