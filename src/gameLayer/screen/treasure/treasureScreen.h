#pragma once
#include "assets/assets.h"
#include "relics/relic.h"
#include "screen/screenAction.h"

ScreenAction UpdateTreasureScreen();

void DrawTreasureScreen(bool hasRelic, RelicId relic, const Assets &assets);
