#include <raylib.h>
#include "mapScreen.h"
#include "gameConfig.h"

// Layout
const float roomRadius = 32.0f;
const float floorGap = 110.0f;
const float columnGap = 240.0f;
const float bottomY = 1000.0f;

// Room position
static Vector2 GetRoomPosition(const Room &room)
{
    float x = screenWidth / 2.0f;
    if (room.IsOnGrid())
    {
        x += (room.column - (mapColumns - 1) / 2.0f) * columnGap;
    }
    float y = bottomY - (room.floor + 1) * floorGap;
    return {x, y};
}

// Room label
static const char *GetRoomLabel(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return "S";
    case RoomType::Combat:
        return "C";
    case RoomType::Rest:
        return "R";
    case RoomType::Boss:
        return "B";
    }

    return "?";
}

// Room color
static Color GetRoomColor(const DungeonMap &map, int currentRoom, int roomId)
{
    if (roomId == currentRoom)
    {
        return GOLD;
    }
    if (map.GetRoom(roomId).cleared)
    {
        return GRAY;
    }
    if (map.CanVisit(currentRoom, roomId))
    {
        return GREEN;
    }

    return LIGHTGRAY;
}

// Update
ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return ScreenAction::None;
    }

    Vector2 mouse = GetMousePosition();
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        Vector2 position = GetRoomPosition(map.GetRoom(id));
        if (CheckCollisionPointCircle(mouse, position, roomRadius) && map.CanVisit(currentRoom, id))
        {
            chosenRoom = id;
            return ScreenAction::RoomChosen;
        }
    }

    return ScreenAction::None;
}

// Draw
void DrawMapScreen(const DungeonMap &map, int currentRoom)
{
    DrawText("MAP", 20, 20, 40, RED);

    // Edges
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        const Room &room = map.GetRoom(id);
        for (int nextId : room.next)
        {
            DrawLineEx(GetRoomPosition(room), GetRoomPosition(map.GetRoom(nextId)), 4.0f, DARKGRAY);
        }
    }

    // Rooms
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        const Room &room = map.GetRoom(id);
        Vector2 position = GetRoomPosition(room);
        DrawCircleV(position, roomRadius, GetRoomColor(map, currentRoom, id));
        DrawCircleLinesV(position, roomRadius, BLACK);

        const char *label = GetRoomLabel(room.type);
        int labelWidth = MeasureText(label, 30);
        DrawText(label, static_cast<int>(position.x) - labelWidth / 2, static_cast<int>(position.y) - 15, 30, BLACK);
    }
}
