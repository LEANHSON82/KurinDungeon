#pragma once
#include "assets/assets.h"
#include "run/player.h"
#include "screen/screenAction.h"

ScreenAction UpdateRestScreen();

void DrawRestScreen(const PlayerState &player, const Assets &assets);
