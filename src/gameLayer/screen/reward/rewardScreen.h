#pragma once
#include <vector>
#include "assets/assets.h"
#include "cards/card.h"
#include "relics/relic.h"
#include "screen/screenAction.h"

ScreenAction UpdateRewardScreen(const std::vector<CardId> &choices, int &chosenIndex);

void DrawRewardScreen(const std::vector<CardId> &choices, int gold, bool hasRelic, RelicId relic, const Assets &assets);
