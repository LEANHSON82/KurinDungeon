#pragma once
#include "map/dungeonMap.h"
#include "screen/screenAction.h"

// Update
ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom);

// Draw
void DrawMapScreen(const DungeonMap &map, int currentRoom);
