#include "scenes/map/Scene_Map.h"
#include <algorithm>
#include <cmath>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_Rest.h"
#include "scenes/battle/Scene_Battle.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static constexpr float roomRadius = 48.0f;
static constexpr float floorGap = 114.0f;
static constexpr float columnGap = 220.0f;
static constexpr float bottomY = 1004.0f;
static constexpr float mapCenterX = 960.0f;

// The map is one big sheet of paper in the middle of the screen; the player's HP and the
// legend sit beside it.
static constexpr Rectangle paperRect{440.0f, 16.0f, 1040.0f, 1048.0f};
static constexpr Rectangle legendRect{1530.0f, 270.0f, 330.0f, 540.0f};
static constexpr Rectangle healthPanel{60.0f, 40.0f, 320.0f, 120.0f};
static constexpr float roomIconScale = 4.0f;
static constexpr float bossIconScale = 6.0f;
// The paper art is nearly white; this tint turns it into old parchment.
static constexpr Color paperTint{242, 200, 164, 255};
static constexpr Color backdropTop{46, 34, 60, 255};
static constexpr Color backdropBottom{14, 10, 20, 255};
static constexpr Color walkedColor{173, 47, 69, 255};
static constexpr Color goldRing{226, 122, 36, 255};
static constexpr Color clearedTint{170, 150, 150, 200};
static constexpr Color lockedTint{120, 100, 110, 200};

Scene_Map::Scene_Map(GameEngine *game) : Scene(game)
{
    MapGenerator generator(m_entityManager);
    m_start = generator.generateValidMap(game->currentRun().seed);
    m_rooms = generator.rooms();
    m_currentRoom = m_start;
    layoutRooms();
    m_entityManager.update();
}

// ---------------------------------------------------------------------------
// Screen layout

// The column halfway between the leftmost and rightmost room, so the map is drawn centered.
static float MiddleColumn(const EntityVec &rooms)
{
    int lowest = mapColumns;
    int highest = -1;
    for (const std::shared_ptr<Entity> &entity : rooms)
    {
        const CRoom &room = entity->get<CRoom>();
        if (IsOnGrid(room))
        {
            lowest = std::min(lowest, room.column);
            highest = std::max(highest, room.column);
        }
    }
    return (lowest + highest) / 2.0f;
}

// The average x of the rooms on one floor, or fallback when that floor has no rooms.
static float FloorCenterX(const EntityVec &rooms, int floor, float fallback)
{
    float total = 0.0f;
    int count = 0;
    for (const std::shared_ptr<Entity> &entity : rooms)
    {
        if (entity->get<CRoom>().floor == floor)
        {
            total += entity->get<CTransform>().pos.x;
            count++;
        }
    }
    return count > 0 ? total / count : fallback;
}

void Scene_Map::layoutRooms()
{
    float middleColumn = MiddleColumn(m_rooms);
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        const CRoom &room = entity->get<CRoom>();
        float x = mapCenterX + (room.column - middleColumn) * columnGap;
        float y = bottomY - (room.floor + 1) * floorGap;
        entity->add<CTransform>(Vector2{x, y});
    }

    // The start and boss rooms have no column of their own: center them under and over
    // the rooms they link to.
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        RoomType type = entity->get<CRoom>().type;
        Vector2 &position = entity->get<CTransform>().pos;
        if (type == RoomType::Start)
        {
            position.x = FloorCenterX(m_rooms, 0, position.x);
        }
        if (type == RoomType::Boss)
        {
            position.x = FloorCenterX(m_rooms, mapFloors - 1, position.x);
        }
    }
}

// ---------------------------------------------------------------------------
// Rules

bool Scene_Map::canVisit(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to) const
{
    if (!from || !to || !from->get<CRoom>().cleared || to->get<CRoom>().cleared)
    {
        return false;
    }
    return HasLink(from, to);
}

void Scene_Map::enterRoom(const std::shared_ptr<Entity> &room)
{
    if (!canVisit(m_currentRoom, room))
    {
        return;
    }

    const CRoom &data = room->get<CRoom>();
    switch (data.type)
    {
    case RoomType::Combat:
    case RoomType::Boss:
        m_currentRoom = room;
        m_game->changeScene("BATTLE", std::make_shared<Scene_Battle>(m_game, data.enemies, data.type == RoomType::Boss));
        break;
    case RoomType::Rest:
        m_currentRoom = room;
        m_game->changeScene("REST", std::make_shared<Scene_Rest>(m_game));
        break;
    case RoomType::Start:
        break;
    }
}

void Scene_Map::finishCurrentRoom()
{
    m_currentRoom->get<CRoom>().cleared = true;
    m_game->currentRun().roomFinished = false;
}

void Scene_Map::update()
{
    m_entityManager.update();
    if (m_game->currentRun().roomFinished)
    {
        finishCurrentRoom();
    }

    sRender();
    m_currentFrame++;
}

void Scene_Map::sDoAction(const Action &action)
{
    if (!action.isStart("LEFT_CLICK"))
    {
        return;
    }
    std::shared_ptr<Entity> room = visitableRoomAt(action.pos());
    if (room)
    {
        enterRoom(room);
    }
}

// ---------------------------------------------------------------------------
// Drawing

// Every room icon is the same size, except the boss, which is the goal of the map.
static float RoomIconScale(RoomType type)
{
    return type == RoomType::Boss ? bossIconScale : roomIconScale;
}

static const char *RoomIcon(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return "RoomStart";
    case RoomType::Combat:
        return "RoomCombat";
    case RoomType::Rest:
        return "RoomRest";
    case RoomType::Boss:
        return "RoomBoss";
    }
    return "RoomCombat";
}

static const char *RoomName(RoomType type)
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

std::shared_ptr<Entity> Scene_Map::roomAt(Vector2 point) const
{
    for (const std::shared_ptr<Entity> &room : m_rooms)
    {
        if (CheckCollisionPointCircle(point, room->get<CTransform>().pos, roomRadius))
        {
            return room;
        }
    }
    return nullptr;
}

std::shared_ptr<Entity> Scene_Map::visitableRoomAt(Vector2 point) const
{
    std::shared_ptr<Entity> room = roomAt(point);
    return canVisit(m_currentRoom, room) ? room : nullptr;
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

std::string Scene_Map::describeRoom(const std::shared_ptr<Entity> &room) const
{
    const CRoom &data = room->get<CRoom>();
    switch (data.type)
    {
    case RoomType::Start:
        return "Your journey begins";
    case RoomType::Combat:
    case RoomType::Boss:
        return DescribeEnemies(data.enemies);
    case RoomType::Rest:
        return "Heal " + std::to_string(restHeal) + " HP";
    }
    return "";
}

const char *Scene_Map::describeState(const std::shared_ptr<Entity> &room) const
{
    if (room == m_currentRoom)
    {
        return "You are here";
    }
    if (canVisit(m_currentRoom, room))
    {
        return "Click to enter";
    }
    if (room->get<CRoom>().cleared)
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

void Scene_Map::drawPaths() const
{
    for (const std::shared_ptr<Entity> &room : m_rooms)
    {
        Vector2 from = room->get<CTransform>().pos;
        for (const std::shared_ptr<Entity> &next : room->get<CRoom>().next)
        {
            Vector2 to = next->get<CTransform>().pos;
            bool walked = room->get<CRoom>().cleared && next->get<CRoom>().cleared;
            bool choice = room == m_currentRoom && canVisit(m_currentRoom, next);
            if (walked)
            {
                DrawPath(from, to, 12, walkedColor);
            }
            else if (choice)
            {
                DrawPath(from, to, 10, inkColor);
            }
            else
            {
                DrawPath(from, to, 6, Fade(inkColor, 0.55f));
            }
        }
    }
}

static void DrawRoomIcon(const Assets &assets, RoomType type, Vector2 position, float grow, Color tint)
{
    DrawIcon(assets.getTexture(RoomIcon(type)), position, RoomIconScale(type) * grow, tint);
}

static void DrawRoomRing(Vector2 center, float innerRadius, float outerRadius, Color color)
{
    DrawRing(center, innerRadius, outerRadius, 0.0f, 360.0f, 32, color);
}

// A room is drawn in one of four ways: the room the player is in, a room that can be
// entered now, a room already cleared, and a room that is still out of reach.
void Scene_Map::drawRoom(const std::shared_ptr<Entity> &entity, bool hovered, float wave) const
{
    const Assets &assets = m_game->assets();
    const CRoom &room = entity->get<CRoom>();
    Vector2 position = entity->get<CTransform>().pos;

    if (entity == m_currentRoom)
    {
        // The player's own room shows the hero's helmet in a dark ring, so it is never
        // mistaken for a room that can be picked next.
        DrawRoomRing(position, roomRadius - 6.0f, roomRadius, inkColor);
        DrawIcon(assets.getTexture("MapHero"), position, roomIconScale, WHITE);
        return;
    }
    if (canVisit(m_currentRoom, entity))
    {
        // It pulses gently to invite a click; under the mouse it stays big and bright.
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

void Scene_Map::drawRooms(const std::shared_ptr<Entity> &hovered) const
{
    // Swings between -1 and 1 a little under once a second.
    float wave = std::sin(static_cast<float>(m_currentFrame) / framesPerSecond * 5.0f);
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        drawRoom(entity, entity == hovered, wave);
    }
}

void Scene_Map::drawRoomInfo(const std::shared_ptr<Entity> &room) const
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    const float width = 400.0f;
    const float height = 180.0f;
    const float gap = 80.0f;          // between the room and its box
    const float screenMargin = 20.0f; // the box keeps this far from the screen edge

    // The box sits to the right of the room, or to the left when there is no space.
    Vector2 position = room->get<CTransform>().pos;
    float x = position.x + gap;
    if (x + width > screenWidth - screenMargin)
    {
        x = position.x - gap - width;
    }
    float y = std::clamp(position.y - height / 2.0f, screenMargin, screenHeight - height - screenMargin);
    Rectangle box{x, y, width, height};
    DrawPanel(assets, box);

    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(font, RoomName(room->get<CRoom>().type), {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(font, describeRoom(room).c_str(), {centerX, box.y + 96.0f}, smallText, textColor);
    DrawTextCenter(font, describeState(room), {centerX, box.y + 140.0f}, smallText, peachColor);
}

static void DrawLegendRow(const Assets &assets, const char *icon, const char *name, float y)
{
    DrawIcon(assets.getTexture(icon), {legendRect.x + 70.0f, y}, roomIconScale, WHITE);
    DrawTextLeft(assets.getFont("Main"), name, {legendRect.x + 126.0f, y - 22.0f}, smallText, inkColor);
}

void Scene_Map::drawLegend() const
{
    const Assets &assets = m_game->assets();
    DrawBox(assets.getBox("MapPaper"), legendRect, paperTint);
    DrawTextCenter(assets.getFont("Main"), "Legend", {legendRect.x + legendRect.width / 2.0f, legendRect.y + 56.0f}, bigText, inkColor);

    const float rowHeight = 76.0f;
    float y = legendRect.y + 136.0f;
    DrawLegendRow(assets, "MapHero", "You", y);
    for (RoomType type : {RoomType::Combat, RoomType::Rest, RoomType::Boss, RoomType::Start})
    {
        y += rowHeight;
        DrawLegendRow(assets, RoomIcon(type), RoomName(type), y);
    }
}

void Scene_Map::sRender()
{
    const Assets &assets = m_game->assets();
    const Run &run = m_game->currentRun();

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, backdropTop, backdropBottom);
    DrawBox(assets.getBox("MapPaper"), paperRect, paperTint);
    drawPaths();
    drawRooms(visitableRoomAt(GetMousePosition()));
    drawLegend();

    DrawPanel(assets, healthPanel);
    DrawIcon(assets.getTexture("Heart"), {healthPanel.x + 66.0f, healthPanel.y + 60.0f}, iconScale, WHITE);
    DrawTextLeft(assets.getFont("Main"), TextFormat("%d / %d", run.hp, run.maxHp), {healthPanel.x + 110.0f, healthPanel.y + 30.0f}, bigText, textColor);

    std::shared_ptr<Entity> pointed = roomAt(GetMousePosition());
    if (pointed)
    {
        drawRoomInfo(pointed);
    }
}
