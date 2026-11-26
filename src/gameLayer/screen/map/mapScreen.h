#pragma once
#include "assets/assets.h"
#include "map/dungeonMap.h"
#include "run/player.h"
#include "screen/screenAction.h"

ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom);

void DrawMapScreen(const DungeonMap &map, int currentRoom, const PlayerState &player, const Assets &assets);
