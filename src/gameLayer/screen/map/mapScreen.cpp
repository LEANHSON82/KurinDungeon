#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <raylib.h>
#include "mapScreen.h"
#include "gameConfig.h"
#include "combat/enemies/enemyDatabase.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float roomRadius = 48.0f;
const float floorGap = 112.0f;
const float columnGap = 170.0f;
const float bottomY = 1000.0f;
const float centerX = 900.0f;

const Rectangle paperRect{460.0f, 24.0f, 880.0f, 1032.0f};
const Rectangle legendRect{1420.0f, 300.0f, 400.0f, 420.0f};
const Color walkedColor{173, 47, 69, 255};
const Color goldRing{240, 181, 65, 255};

static std::vector<Vector2> GetRoomPositions(const DungeonMap &map)
{
    int count = map.GetRoomCount();
    std::vector<Vector2> positions(count);

    int lowest = mapColumns;
    int highest = -1;
    for (int id = 0; id < count; id++)
    {
        const Room &room = map.GetRoom(id);
        if (room.IsOnGrid())
        {
            lowest = std::min(lowest, room.column);
            highest = std::max(highest, room.column);
        }
    }
    float middleColumn = (lowest + highest) / 2.0f;

    float firstFloorX = 0.0f;
    float lastFloorX = 0.0f;
    int firstFloorCount = 0;
    int lastFloorCount = 0;
    for (int id = 0; id < count; id++)
    {
        const Room &room = map.GetRoom(id);
        float x = centerX + (room.column - middleColumn) * columnGap;
        float y = bottomY - (room.floor + 1) * floorGap;
        positions[id] = {x, y};

        if (room.IsOnGrid() && room.floor == 0)
        {
            firstFloorX += x;
            firstFloorCount++;
        }
        if (room.IsOnGrid() && room.floor == mapFloors - 1)
        {
            lastFloorX += x;
            lastFloorCount++;
        }
    }

    for (int id = 0; id < count; id++)
    {
        const Room &room = map.GetRoom(id);
        if (room.type == RoomType::Start)
        {
            positions[id].x = firstFloorX / firstFloorCount;
        }
        if (room.type == RoomType::Boss)
        {
            positions[id].x = lastFloorX / lastFloorCount;
        }
    }
    return positions;
}

static float GetRoomIconScale(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
    case RoomType::Rest:
        return detailScale;
    case RoomType::Combat:
        return uiScale;
    case RoomType::Boss:
        return 10.0f;
    }
    return uiScale;
}

static const char *GetRoomName(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return "Start";
    case RoomType::Combat:
        return "Enemy";
    case RoomType::Rest:
        return "Rest";
    case RoomType::Boss:
        return "Boss";
    }
    return "";
}

static int GetRoomAtMouse(const DungeonMap &map, const std::vector<Vector2> &positions)
{
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        if (CheckCollisionPointCircle(GetMousePosition(), positions[id], roomRadius))
        {
            return id;
        }
    }
    return -1;
}

static int GetRoomUnderMouse(const DungeonMap &map, int currentRoom, const std::vector<Vector2> &positions)
{
    int room = GetRoomAtMouse(map, positions);
    return room != -1 && map.CanVisit(currentRoom, room) ? room : -1;
}

static std::string DescribeEnemies(const std::vector<EnemyId> &enemies)
{
    std::string text;
    std::vector<EnemyId> counted;
    for (EnemyId id : enemies)
    {
        if (std::find(counted.begin(), counted.end(), id) != counted.end())
        {
            continue;
        }
        counted.push_back(id);

        int count = static_cast<int>(std::count(enemies.begin(), enemies.end(), id));
        if (!text.empty())
        {
            text += ", ";
        }
        text += GetEnemyDefinition(id).name;
        if (count > 1)
        {
            text += " x" + std::to_string(count);
        }
    }
    return text;
}

static std::string DescribeRoom(const Room &room)
{
    switch (room.type)
    {
    case RoomType::Start:
        return "Your journey begins";
    case RoomType::Combat:
    case RoomType::Boss:
        return DescribeEnemies(room.enemies);
    case RoomType::Rest:
        return "Heal " + std::to_string(restHeal) + " HP";
    }
    return "";
}

static const char *DescribeState(const DungeonMap &map, int currentRoom, int roomId)
{
    if (roomId == currentRoom)
    {
        return "You are here";
    }
    if (map.CanVisit(currentRoom, roomId))
    {
        return "Click to enter";
    }
    if (map.GetRoom(roomId).cleared)
    {
        return "Cleared";
    }
    return "Not reachable yet";
}

static void DrawPath(Vector2 from, Vector2 to, int dotSize, Color color)
{
    const float step = 22.0f;
    float length = std::sqrt((to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y));
    int dots = static_cast<int>(length / step);
    for (int i = 2; i < dots - 1; i++)
    {
        float t = static_cast<float>(i) / dots;
        int x = static_cast<int>(from.x + (to.x - from.x) * t);
        int y = static_cast<int>(from.y + (to.y - from.y) * t);
        DrawRectangle(x - dotSize / 2, y - dotSize / 2, dotSize, dotSize, color);
    }
}

static void DrawPaths(const DungeonMap &map, int currentRoom, const std::vector<Vector2> &positions)
{
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        for (int nextId : map.GetRoom(id).next)
        {
            bool walked = map.GetRoom(id).cleared && map.GetRoom(nextId).cleared;
            bool choice = id == currentRoom && map.CanVisit(currentRoom, nextId);
            if (walked)
            {
                DrawPath(positions[id], positions[nextId], 12, walkedColor);
            }
            else if (choice)
            {
                DrawPath(positions[id], positions[nextId], 10, inkColor);
            }
            else
            {
                DrawPath(positions[id], positions[nextId], 6, Fade(inkColor, 0.3f));
            }
        }
    }
}

static void DrawRoom(const Assets &assets, RoomType type, Vector2 position, float grow, Color tint)
{
    DrawIcon(GetRoomIcon(assets, type), position, GetRoomIconScale(type) * grow, tint);
}

static void DrawRooms(const Assets &assets, const DungeonMap &map, int currentRoom, const std::vector<Vector2> &positions, int hoveredRoom)
{
    float wave = std::sin(static_cast<float>(GetTime()) * 5.0f);

    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        const Room &room = map.GetRoom(id);
        Vector2 position = positions[id];

        if (id == currentRoom)
        {
            DrawRing(position, roomRadius - 4.0f, roomRadius + 6.0f, 0.0f, 360.0f, 32, goldRing);
            DrawRoom(assets, room.type, position, 1.0f, WHITE);
        }
        else if (map.CanVisit(currentRoom, id))
        {
            bool hovered = id == hoveredRoom;
            float glow = hovered ? 0.55f : 0.3f + 0.12f * wave;
            DrawCircleV(position, roomRadius + 10.0f, Fade(goldRing, glow));
            DrawRing(position, roomRadius + 4.0f, roomRadius + 10.0f, 0.0f, 360.0f, 32, goldRing);
            DrawRoom(assets, room.type, position, hovered ? 1.3f : 1.0f + 0.08f * wave, WHITE);
        }
        else if (room.cleared)
        {
            DrawRing(position, roomRadius - 6.0f, roomRadius, 0.0f, 360.0f, 32, walkedColor);
            DrawRoom(assets, room.type, position, 1.0f, Color{170, 150, 150, 200});
        }
        else
        {
            DrawRoom(assets, room.type, position, 0.9f, Color{150, 140, 150, 110});
        }
    }
}

static void DrawRoomInfo(const Assets &assets, const DungeonMap &map, int currentRoom, int roomId, Vector2 position)
{
    const float width = 400.0f;
    const float height = 180.0f;
    float x = position.x + 80.0f;
    if (x + width > screenWidth - 20.0f)
    {
        x = position.x - 80.0f - width;
    }
    float y = std::clamp(position.y - height / 2.0f, 20.0f, screenHeight - height - 20.0f);
    Rectangle box{x, y, width, height};
    DrawPanel(assets, box);

    const Room &room = map.GetRoom(roomId);
    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, GetRoomName(room.type), {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(assets.font, DescribeRoom(room).c_str(), {centerX, box.y + 96.0f}, smallText, textColor);
    DrawTextCenter(assets.font, DescribeState(map, currentRoom, roomId), {centerX, box.y + 140.0f}, smallText, peachColor);
}

static void DrawLegend(const Assets &assets)
{
    DrawBox(assets.mapPaper, legendRect, WHITE);
    DrawTextCenter(assets.font, "Legend", {legendRect.x + legendRect.width / 2.0f, legendRect.y + 60.0f}, bigText, inkColor);

    const RoomType types[]{RoomType::Combat, RoomType::Rest, RoomType::Boss, RoomType::Start};
    for (int i = 0; i < 4; i++)
    {
        float y = legendRect.y + 140.0f + i * 70.0f;
        float iconScale = types[i] == RoomType::Boss ? uiScale : GetRoomIconScale(types[i]);
        DrawIcon(GetRoomIcon(assets, types[i]), {legendRect.x + 80.0f, y}, iconScale, WHITE);
        DrawTextLeft(assets.font, GetRoomName(types[i]), {legendRect.x + 140.0f, y - 26.0f}, bigText, inkColor);
    }
}

ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom)
{
    int room = GetRoomUnderMouse(map, currentRoom, GetRoomPositions(map));
    if (room != -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        chosenRoom = room;
        return ScreenAction::RoomChosen;
    }
    return ScreenAction::None;
}

void DrawMapScreen(const DungeonMap &map, int currentRoom, const PlayerState &player, const Assets &assets)
{
    std::vector<Vector2> positions = GetRoomPositions(map);
    int hoveredRoom = GetRoomUnderMouse(map, currentRoom, positions);
    int pointedRoom = GetRoomAtMouse(map, positions);

    ClearBackground(shadeColor);
    DrawBox(assets.mapPaper, paperRect, WHITE);
    DrawPaths(map, currentRoom, positions);
    DrawRooms(assets, map, currentRoom, positions, hoveredRoom);
    DrawLegend(assets);

    Rectangle info{60.0f, 60.0f, 340.0f, 120.0f};
    DrawPanel(assets, info);
    DrawIcon(assets.heartIcon, {info.x + 66.0f, info.y + 60.0f}, uiScale, WHITE);
    DrawTextLeft(assets.font, TextFormat("%d / %d", player.hp, player.maxHp), {info.x + 110.0f, info.y + 30.0f}, bigText, textColor);

    if (pointedRoom != -1)
    {
        DrawRoomInfo(assets, map, currentRoom, pointedRoom, positions[pointedRoom]);
    }
}
