#pragma once
#include "assets/assets.h"
#include "map/dungeonMap.h"
#include "run/runState.h"
#include "screen/screenAction.h"

ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom);

void DrawMapScreen(const RunState &run, const Assets &assets);
