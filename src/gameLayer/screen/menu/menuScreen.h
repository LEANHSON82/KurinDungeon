#pragma once
#include "assets/assets.h"
#include "screen/screenAction.h"

ScreenAction UpdateMenuScreen(bool hasSave);

void DrawMenuScreen(bool hasSave, const Assets &assets);
