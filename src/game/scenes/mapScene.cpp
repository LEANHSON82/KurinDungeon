#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <raylib.h>
#include "core/components/combat.h"
#include "core/components/map.h"
#include "core/state/run.h"
#include "core/systems/run/mapRules.h"
#include "game/assets/assets.h"
#include "game/components/visual.h"
#include "game/gameConfig.h"
#include "game/scenes/scenes.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static constexpr float roomRadius = 48.0f;
static constexpr float floorGap = 112.0f;
static constexpr float columnGap = 170.0f;
static constexpr float bottomY = 1000.0f;
static constexpr float mapCenterX = 900.0f;

static constexpr Rectangle paperRect{460.0f, 24.0f, 880.0f, 1032.0f};
static constexpr Rectangle legendRect{1420.0f, 300.0f, 400.0f, 420.0f};
static constexpr Color walkedColor{173, 47, 69, 255};
static constexpr Color goldRing{240, 181, 65, 255};
static constexpr Color clearedTint{170, 150, 150, 200};
static constexpr Color lockedTint{150, 140, 150, 110};

void LayoutMapScene(ecs::Registry &registry)
{
    std::vector<ecs::Entity> rooms = registry.View<Room>().Collect();

    int lowest = mapColumns;
    int highest = -1;
    for (ecs::Entity entity : rooms)
    {
        const Room &room = registry.Get<Room>(entity);
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
    for (ecs::Entity entity : rooms)
    {
        const Room &room = registry.Get<Room>(entity);
        float x = mapCenterX + (room.column - middleColumn) * columnGap;
        float y = bottomY - (room.floor + 1) * floorGap;
        registry.EmplaceOrReplace<MapPosition>(entity, Vector2{x, y});

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

    for (ecs::Entity entity : rooms)
    {
        RoomType type = registry.Get<Room>(entity).type;
        if (type == RoomType::Start && firstFloorCount > 0)
        {
            registry.Get<MapPosition>(entity).value.x = firstFloorX / firstFloorCount;
        }
        if (type == RoomType::Boss && lastFloorCount > 0)
        {
            registry.Get<MapPosition>(entity).value.x = lastFloorX / lastFloorCount;
        }
    }
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

static ecs::Entity GetRoomAtMouse(const ecs::Registry &registry)
{
    for (ecs::Entity room : registry.View<Room, MapPosition>().Collect())
    {
        if (CheckCollisionPointCircle(GetMousePosition(), registry.Get<MapPosition>(room).value, roomRadius))
        {
            return room;
        }
    }
    return ecs::nullEntity;
}

static ecs::Entity GetVisitableRoomAtMouse(const ecs::Registry &registry)
{
    ecs::Entity room = GetRoomAtMouse(registry);
    return CanVisit(registry, registry.Resource<RunState>().currentRoom, room) ? room : ecs::nullEntity;
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

static std::string DescribeRoom(const ecs::Registry &registry, ecs::Entity room)
{
    switch (registry.Get<Room>(room).type)
    {
    case RoomType::Start:
        return "Your journey begins";
    case RoomType::Combat:
    case RoomType::Boss:
        return DescribeEnemies(registry.Get<Encounter>(room).enemies);
    case RoomType::Rest:
        return "Heal " + std::to_string(restHeal) + " HP";
    }
    return "";
}

static const char *DescribeState(const ecs::Registry &registry, ecs::Entity room)
{
    ecs::Entity currentRoom = registry.Resource<RunState>().currentRoom;
    if (room == currentRoom)
    {
        return "You are here";
    }
    if (CanVisit(registry, currentRoom, room))
    {
        return "Click to enter";
    }
    if (registry.Has<Cleared>(room))
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

static void DrawPaths(const ecs::Registry &registry)
{
    ecs::Entity currentRoom = registry.Resource<RunState>().currentRoom;
    registry.View<RoomLinks, MapPosition>().Each([&](ecs::Entity room, const RoomLinks &links, const MapPosition &from) {
        for (ecs::Entity next : links.next)
        {
            Vector2 to = registry.Get<MapPosition>(next).value;
            bool walked = registry.Has<Cleared>(room) && registry.Has<Cleared>(next);
            bool choice = room == currentRoom && CanVisit(registry, currentRoom, next);
            if (walked)
            {
                DrawPath(from.value, to, 12, walkedColor);
            }
            else if (choice)
            {
                DrawPath(from.value, to, 10, inkColor);
            }
            else
            {
                DrawPath(from.value, to, 6, Fade(inkColor, 0.3f));
            }
        }
    });
}

static void DrawRoomIcon(const Assets &assets, RoomType type, Vector2 position, float grow, Color tint)
{
    DrawIcon(GetRoomIcon(assets, type), position, GetRoomIconScale(type) * grow, tint);
}

static void DrawRooms(const ecs::Registry &registry, const Assets &assets, ecs::Entity hoveredRoom)
{
    float wave = std::sin(static_cast<float>(GetTime()) * 5.0f);
    ecs::Entity currentRoom = registry.Resource<RunState>().currentRoom;

    registry.View<Room, MapPosition>().Each([&](ecs::Entity entity, const Room &room, const MapPosition &mapPosition) {
        Vector2 position = mapPosition.value;
        if (entity == currentRoom)
        {
            DrawRing(position, roomRadius - 4.0f, roomRadius + 6.0f, 0.0f, 360.0f, 32, goldRing);
            DrawRoomIcon(assets, room.type, position, 1.0f, WHITE);
        }
        else if (CanVisit(registry, currentRoom, entity))
        {
            bool hovered = entity == hoveredRoom;
            float glow = hovered ? 0.55f : 0.3f + 0.12f * wave;
            DrawCircleV(position, roomRadius + 10.0f, Fade(goldRing, glow));
            DrawRing(position, roomRadius + 4.0f, roomRadius + 10.0f, 0.0f, 360.0f, 32, goldRing);
            DrawRoomIcon(assets, room.type, position, hovered ? 1.3f : 1.0f + 0.08f * wave, WHITE);
        }
        else if (registry.Has<Cleared>(entity))
        {
            DrawRing(position, roomRadius - 6.0f, roomRadius, 0.0f, 360.0f, 32, walkedColor);
            DrawRoomIcon(assets, room.type, position, 1.0f, clearedTint);
        }
        else
        {
            DrawRoomIcon(assets, room.type, position, 0.9f, lockedTint);
        }
    });
}

static void DrawRoomInfo(const ecs::Registry &registry, const Assets &assets, ecs::Entity room)
{
    const float width = 400.0f;
    const float height = 180.0f;
    Vector2 position = registry.Get<MapPosition>(room).value;
    float x = position.x + 80.0f;
    if (x + width > screenWidth - 20.0f)
    {
        x = position.x - 80.0f - width;
    }
    float y = std::clamp(position.y - height / 2.0f, 20.0f, screenHeight - height - 20.0f);
    Rectangle box{x, y, width, height};
    DrawPanel(assets, box);

    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, GetRoomName(registry.Get<Room>(room).type), {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(assets.font, DescribeRoom(registry, room).c_str(), {centerX, box.y + 96.0f}, smallText, textColor);
    DrawTextCenter(assets.font, DescribeState(registry, room), {centerX, box.y + 140.0f}, smallText, peachColor);
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

void UpdateMapScene(ecs::Registry &registry)
{
    ecs::Entity room = GetVisitableRoomAtMouse(registry);
    if (!room.IsNull() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        PushRunCommand(registry, EnterRoomCommand(room));
    }
}

void DrawMapScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    const Health &health = registry.Get<Health>(registry.Resource<RunState>().player);

    ClearBackground(shadeColor);
    DrawBox(assets.mapPaper, paperRect, WHITE);
    DrawPaths(registry);
    DrawRooms(registry, assets, GetVisitableRoomAtMouse(registry));
    DrawLegend(assets);

    Rectangle info{60.0f, 60.0f, 340.0f, 120.0f};
    DrawPanel(assets, info);
    DrawIcon(assets.heartIcon, {info.x + 66.0f, info.y + 60.0f}, uiScale, WHITE);
    DrawTextLeft(assets.font, TextFormat("%d / %d", health.hp, health.maxHp), {info.x + 110.0f, info.y + 30.0f}, bigText, textColor);

    ecs::Entity pointedRoom = GetRoomAtMouse(registry);
    if (!pointedRoom.IsNull())
    {
        DrawRoomInfo(registry, assets, pointedRoom);
    }
}
