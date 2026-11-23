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

void Scene_Map::layoutRooms()
{
    int lowest = mapColumns;
    int highest = -1;
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        const CRoom &room = entity->get<CRoom>();
        if (IsOnGrid(room))
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
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        const CRoom &room = entity->get<CRoom>();
        float x = mapCenterX + (room.column - middleColumn) * columnGap;
        float y = bottomY - (room.floor + 1) * floorGap;
        entity->add<CTransform>(Vector2{x, y});
        if (IsOnGrid(room) && room.floor == 0)
        {
            firstFloorX += x;
            firstFloorCount++;
        }
        if (IsOnGrid(room) && room.floor == mapFloors - 1)
        {
            lastFloorX += x;
            lastFloorCount++;
        }
    }

    // Center the start and boss rooms under and over the rooms they link to.
    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        RoomType type = entity->get<CRoom>().type;
        if (type == RoomType::Start && firstFloorCount > 0)
        {
            entity->get<CTransform>().pos.x = firstFloorX / firstFloorCount;
        }
        if (type == RoomType::Boss && lastFloorCount > 0)
        {
            entity->get<CTransform>().pos.x = lastFloorX / lastFloorCount;
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
    if (action.name() == "LEFT_CLICK" && action.type() == "START")
    {
        std::shared_ptr<Entity> room = visitableRoomAt(action.pos());
        if (room)
        {
            enterRoom(room);
        }
    }
}

// ---------------------------------------------------------------------------
// Drawing

static float RoomIconScale(RoomType type)
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

static const char *RoomIcon(RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return "RoomStart";
    case RoomType::Combat:
        return "Sword";
    case RoomType::Rest:
        return "RoomRest";
    case RoomType::Boss:
        return "Skull";
    }
    return "Sword";
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
                DrawPath(from, to, 6, Fade(inkColor, 0.3f));
            }
        }
    }
}

void Scene_Map::drawRooms(const std::shared_ptr<Entity> &hovered) const
{
    const Assets &assets = m_game->assets();
    float wave = std::sin(static_cast<float>(m_currentFrame) / framesPerSecond * 5.0f);
    auto drawIcon = [&](RoomType type, Vector2 position, float grow, Color tint) {
        DrawIcon(assets.getTexture(RoomIcon(type)), position, RoomIconScale(type) * grow, tint);
    };

    for (const std::shared_ptr<Entity> &entity : m_rooms)
    {
        const CRoom &room = entity->get<CRoom>();
        Vector2 position = entity->get<CTransform>().pos;
        if (entity == m_currentRoom)
        {
            DrawRing(position, roomRadius - 4.0f, roomRadius + 6.0f, 0.0f, 360.0f, 32, goldRing);
            drawIcon(room.type, position, 1.0f, WHITE);
        }
        else if (canVisit(m_currentRoom, entity))
        {
            bool isHovered = entity == hovered;
            float glow = isHovered ? 0.55f : 0.3f + 0.12f * wave;
            DrawCircleV(position, roomRadius + 10.0f, Fade(goldRing, glow));
            DrawRing(position, roomRadius + 4.0f, roomRadius + 10.0f, 0.0f, 360.0f, 32, goldRing);
            drawIcon(room.type, position, isHovered ? 1.3f : 1.0f + 0.08f * wave, WHITE);
        }
        else if (room.cleared)
        {
            DrawRing(position, roomRadius - 6.0f, roomRadius, 0.0f, 360.0f, 32, walkedColor);
            drawIcon(room.type, position, 1.0f, clearedTint);
        }
        else
        {
            drawIcon(room.type, position, 0.9f, lockedTint);
        }
    }
}

void Scene_Map::drawRoomInfo(const std::shared_ptr<Entity> &room) const
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    const float width = 400.0f;
    const float height = 180.0f;
    Vector2 position = room->get<CTransform>().pos;
    float x = position.x + 80.0f;
    if (x + width > screenWidth - 20.0f)
    {
        x = position.x - 80.0f - width;
    }
    float y = std::clamp(position.y - height / 2.0f, 20.0f, screenHeight - height - 20.0f);
    Rectangle box{x, y, width, height};
    DrawPanel(assets, box);

    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(font, RoomName(room->get<CRoom>().type), {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(font, describeRoom(room).c_str(), {centerX, box.y + 96.0f}, smallText, textColor);
    DrawTextCenter(font, describeState(room), {centerX, box.y + 140.0f}, smallText, peachColor);
}

void Scene_Map::drawLegend() const
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    DrawBox(assets.getBox("MapPaper"), legendRect, WHITE);
    DrawTextCenter(font, "Legend", {legendRect.x + legendRect.width / 2.0f, legendRect.y + 60.0f}, bigText, inkColor);

    const RoomType types[]{RoomType::Combat, RoomType::Rest, RoomType::Boss, RoomType::Start};
    for (int i = 0; i < 4; i++)
    {
        float y = legendRect.y + 140.0f + i * 70.0f;
        float iconScale = types[i] == RoomType::Boss ? uiScale : RoomIconScale(types[i]);
        DrawIcon(assets.getTexture(RoomIcon(types[i])), {legendRect.x + 80.0f, y}, iconScale, WHITE);
        DrawTextLeft(font, RoomName(types[i]), {legendRect.x + 140.0f, y - 26.0f}, bigText, inkColor);
    }
}

void Scene_Map::sRender()
{
    const Assets &assets = m_game->assets();
    const Run &run = m_game->currentRun();

    ClearBackground(shadeColor);
    DrawBox(assets.getBox("MapPaper"), paperRect, WHITE);
    drawPaths();
    drawRooms(visitableRoomAt(GetMousePosition()));
    drawLegend();

    Rectangle info{60.0f, 60.0f, 340.0f, 120.0f};
    DrawPanel(assets, info);
    DrawIcon(assets.getTexture("Heart"), {info.x + 66.0f, info.y + 60.0f}, uiScale, WHITE);
    DrawTextLeft(assets.getFont("Main"), TextFormat("%d / %d", run.hp, run.maxHp), {info.x + 110.0f, info.y + 30.0f}, bigText, textColor);

    std::shared_ptr<Entity> pointed = roomAt(GetMousePosition());
    if (pointed)
    {
        drawRoomInfo(pointed);
    }
}
