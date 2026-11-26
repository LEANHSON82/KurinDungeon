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
const float floorGap = 114.0f;
const float columnGap = 220.0f;
const float bottomY = 1004.0f;
const float mapCenterX = 960.0f;

const Rectangle paperRect{440.0f, 16.0f, 1040.0f, 1048.0f};
const Rectangle legendRect{1530.0f, 270.0f, 330.0f, 540.0f};
const Rectangle healthPanel{60.0f, 40.0f, 320.0f, 120.0f};
const float roomIconScale = 4.0f;
const float bossIconScale = 6.0f;
const float legendRowHeight = 76.0f;

const Color paperTint{242, 200, 164, 255};
const Color backdropTop{46, 34, 60, 255};
const Color backdropBottom{14, 10, 20, 255};
const Color walkedColor{173, 47, 69, 255};
const Color goldRing{226, 122, 36, 255};
const Color clearedTint{170, 150, 150, 200};
const Color lockedTint{120, 100, 110, 200};

static float GetMiddleColumn(const DungeonMap &map)
{
    int lowest = mapColumns;
    int highest = -1;
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        const Room &room = map.GetRoom(id);
        if (room.IsOnGrid())
        {
            lowest = std::min(lowest, room.column);
            highest = std::max(highest, room.column);
        }
    }
    return (lowest + highest) / 2.0f;
}

static float GetFloorCenterX(const DungeonMap &map, const std::vector<Vector2> &positions, int floor, float fallback)
{
    float total = 0.0f;
    int count = 0;
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        if (map.GetRoom(id).floor == floor)
        {
            total += positions[id].x;
            count++;
        }
    }
    return count > 0 ? total / count : fallback;
}

static std::vector<Vector2> GetRoomPositions(const DungeonMap &map)
{
    int count = map.GetRoomCount();
    std::vector<Vector2> positions(count);
    float middleColumn = GetMiddleColumn(map);
    for (int id = 0; id < count; id++)
    {
        const Room &room = map.GetRoom(id);
        float x = mapCenterX + (room.column - middleColumn) * columnGap;
        float y = bottomY - (room.floor + 1) * floorGap;
        positions[id] = {x, y};
    }

    for (int id = 0; id < count; id++)
    {
        RoomType type = map.GetRoom(id).type;
        if (type == RoomType::Start)
        {
            positions[id].x = GetFloorCenterX(map, positions, 0, positions[id].x);
        }
        if (type == RoomType::Boss)
        {
            positions[id].x = GetFloorCenterX(map, positions, mapFloors - 1, positions[id].x);
        }
    }
    return positions;
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

ScreenAction UpdateMapScreen(const DungeonMap &map, int currentRoom, int &chosenRoom)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return ScreenAction::None;
    }

    int room = GetRoomAtMouse(map, GetRoomPositions(map));
    if (!map.CanVisit(currentRoom, room))
    {
        return ScreenAction::None;
    }

    chosenRoom = room;
    return ScreenAction::RoomChosen;
}

static float GetRoomIconScale(RoomType type)
{
    return type == RoomType::Boss ? bossIconScale : roomIconScale;
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

static const char *DescribeState(const DungeonMap &map, int currentRoom, int room)
{
    if (room == currentRoom)
    {
        return "You are here";
    }
    if (map.CanVisit(currentRoom, room))
    {
        return "Click to enter";
    }
    if (map.GetRoom(room).cleared)
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
        const Room &room = map.GetRoom(id);
        for (int next : room.next)
        {
            bool walked = room.cleared && map.GetRoom(next).cleared;
            bool choice = id == currentRoom && map.CanVisit(currentRoom, next);
            if (walked)
            {
                DrawPath(positions[id], positions[next], 12, walkedColor);
            }
            else if (choice)
            {
                DrawPath(positions[id], positions[next], 10, inkColor);
            }
            else
            {
                DrawPath(positions[id], positions[next], 6, Fade(inkColor, 0.55f));
            }
        }
    }
}

static void DrawRoomIcon(const Assets &assets, RoomType type, Vector2 position, float grow, Color tint)
{
    DrawIcon(GetRoomIcon(assets, type), position, GetRoomIconScale(type) * grow, tint);
}

static void DrawRoomRing(Vector2 center, float innerRadius, float outerRadius, Color color)
{
    DrawRing(center, innerRadius, outerRadius, 0.0f, 360.0f, 32, color);
}

static void DrawRoom(const DungeonMap &map, int currentRoom, int id, Vector2 position, bool hovered, float wave, const Assets &assets)
{
    const Room &room = map.GetRoom(id);
    if (id == currentRoom)
    {
        DrawRoomRing(position, roomRadius - 6.0f, roomRadius, inkColor);
        DrawIcon(assets.mapHero, position, roomIconScale, WHITE);
        return;
    }
    if (map.CanVisit(currentRoom, id))
    {
        float glow = hovered ? 0.55f : 0.3f + 0.12f * wave;
        float grow = hovered ? 1.3f : 1.0f + 0.08f * wave;
        DrawCircleV(position, roomRadius + 10.0f, Fade(goldRing, glow));
        DrawRoomRing(position, roomRadius + 4.0f, roomRadius + 10.0f, goldRing);
        DrawRoomIcon(assets, room.type, position, grow, WHITE);
        return;
    }
    if (room.cleared)
    {
        DrawRoomRing(position, roomRadius - 6.0f, roomRadius, walkedColor);
        DrawRoomIcon(assets, room.type, position, 1.0f, clearedTint);
        return;
    }
    DrawRoomIcon(assets, room.type, position, 0.9f, lockedTint);
}

static void DrawRoomInfo(const DungeonMap &map, int currentRoom, int room, Vector2 position, const Assets &assets)
{
    const float width = 400.0f;
    const float height = 180.0f;
    const float gap = 80.0f;
    const float screenMargin = 20.0f;

    float x = position.x + gap;
    if (x + width > screenWidth - screenMargin)
    {
        x = position.x - gap - width;
    }
    float y = std::clamp(position.y - height / 2.0f, screenMargin, screenHeight - height - screenMargin);
    Rectangle box{x, y, width, height};
    DrawPanel(assets, box);

    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, GetRoomName(map.GetRoom(room).type), {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(assets.font, DescribeRoom(map.GetRoom(room)).c_str(), {centerX, box.y + 96.0f}, smallText, textColor);
    DrawTextCenter(assets.font, DescribeState(map, currentRoom, room), {centerX, box.y + 140.0f}, smallText, peachColor);
}

static void DrawLegendRow(const Assets &assets, Texture2D icon, const char *name, float y)
{
    DrawIcon(icon, {legendRect.x + 70.0f, y}, roomIconScale, WHITE);
    DrawTextLeft(assets.font, name, {legendRect.x + 126.0f, y - 22.0f}, smallText, inkColor);
}

static void DrawLegend(const Assets &assets)
{
    DrawBox(assets.mapPaper, legendRect, paperTint);
    DrawTextCenter(assets.font, "Legend", {legendRect.x + legendRect.width / 2.0f, legendRect.y + 56.0f}, bigText, inkColor);

    float y = legendRect.y + 136.0f;
    DrawLegendRow(assets, assets.mapHero, "You", y);
    for (RoomType type : {RoomType::Combat, RoomType::Rest, RoomType::Boss, RoomType::Start})
    {
        y += legendRowHeight;
        DrawLegendRow(assets, GetRoomIcon(assets, type), GetRoomName(type), y);
    }
}

void DrawMapScreen(const DungeonMap &map, int currentRoom, const PlayerState &player, const Assets &assets)
{
    std::vector<Vector2> positions = GetRoomPositions(map);
    int pointed = GetRoomAtMouse(map, positions);
    int hovered = map.CanVisit(currentRoom, pointed) ? pointed : -1;
    float wave = std::sin(static_cast<float>(GetTime()) * 5.0f);

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, backdropTop, backdropBottom);
    DrawBox(assets.mapPaper, paperRect, paperTint);
    DrawPaths(map, currentRoom, positions);
    for (int id = 0; id < map.GetRoomCount(); id++)
    {
        DrawRoom(map, currentRoom, id, positions[id], id == hovered, wave, assets);
    }
    DrawLegend(assets);

    DrawPanel(assets, healthPanel);
    DrawIcon(assets.heartIcon, {healthPanel.x + 66.0f, healthPanel.y + 60.0f}, iconScale, WHITE);
    DrawTextLeft(assets.font, TextFormat("%d / %d", player.hp, player.maxHp), {healthPanel.x + 110.0f, healthPanel.y + 30.0f}, bigText, textColor);

    if (pointed != -1)
    {
        DrawRoomInfo(map, currentRoom, pointed, positions[pointed], assets);
    }
}
