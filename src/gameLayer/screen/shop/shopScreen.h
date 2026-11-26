#pragma once
#include "assets/assets.h"
#include "run/runState.h"
#include "screen/screenAction.h"

ScreenAction UpdateShopScreen(const ShopStock &shop, int gold, int &chosenItem);

void DrawShopScreen(const ShopStock &shop, int gold, const Assets &assets);
