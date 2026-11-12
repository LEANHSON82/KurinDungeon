#include <raylib.h>
#include "mapScreen.h"
#include "combat/enemies/enemyDatabase.h"

// Room name
static const char *GetRoomTypeName(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return "Start";
    case RoomType::Combat:
        return "Combat";
    }

    return "Unknown";
}

// Update
ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom)
{
    const std::vector<int> &next = map.GetRoom(currentRoom).next;
    for (int i = 0; i < static_cast<int>(next.size()); i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
        {
            chosenRoom = next[i];
            return ScreenAction::RoomChosen;
        }
    }

    return ScreenAction::None;
}

// Draw
void DrawMapScreen(const DungeonMap &map, int currentRoom)
{
    const Room &room = map.GetRoom(currentRoom);
    DrawText("MAP", 20, 20, 40, RED);
    DrawText(TextFormat("Current room: %d (%s)", currentRoom, GetRoomTypeName(room.type)), 20, 90, 30, DARKGRAY);

    int y = 150;
    for (int i = 0; i < static_cast<int>(room.next.size()); i++)
    {
        const Room &nextRoom = map.GetRoom(room.next[i]);
        DrawText(TextFormat("[%d] Room %d - %s: %s", i + 1, room.next[i],
                            GetRoomTypeName(nextRoom.type),
                            GetEnemyDefinition(nextRoom.enemy).name),
                 20, y, 30, BLACK);
        y += 45;
    }
}
