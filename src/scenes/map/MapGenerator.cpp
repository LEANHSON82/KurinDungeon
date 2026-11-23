#include "scenes/map/MapGenerator.h"
#include <algorithm>
#include <map>
#include <queue>
#include <raylib.h>

static constexpr int mapAttempts = 16;

bool IsOnGrid(const CRoom &room)
{
    return room.floor >= 0 && room.floor < mapFloors;
}

bool HasLink(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to)
{
    if (!from || !to)
    {
        return false;
    }
    const std::vector<std::shared_ptr<Entity>> &next = from->get<CRoom>().next;
    return std::find(next.begin(), next.end(), to) != next.end();
}

MapGenerator::MapGenerator(EntityManager &entityManager) : m_entityManager(entityManager)
{
}

// ---------------------------------------------------------------------------
// Map generation

std::shared_ptr<Entity> MapGenerator::addRoom(RoomType type, int floor, int column)
{
    std::shared_ptr<Entity> room = m_entityManager.addEntity("room");
    room->add<CRoom>(type, floor, column);
    m_rooms.push_back(room);
    return room;
}

void MapGenerator::addLink(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to)
{
    if (!HasLink(from, to))
    {
        from->get<CRoom>().next.push_back(to);
    }
}

// Picks the column of the next room for a path standing at (fromFloor, column).
int MapGenerator::pickNextColumn(const RoomGrid &grid, std::mt19937 &rng, int fromFloor, int column) const
{
    std::vector<int> choices;
    for (int nextColumn = column - 1; nextColumn <= column + 1; nextColumn++)
    {
        if (nextColumn < 0 || nextColumn >= mapColumns)
        {
            continue;
        }
        // Moving sideways must not cross a link that already goes the other way.
        if (nextColumn != column && HasLink(grid[fromFloor][nextColumn], grid[fromFloor + 1][column]))
        {
            continue;
        }
        choices.push_back(nextColumn);
    }

    std::uniform_int_distribution<int> pick(0, static_cast<int>(choices.size()) - 1);
    return choices[pick(rng)];
}

bool MapGenerator::hasRestParent(const std::shared_ptr<Entity> &room) const
{
    for (const std::shared_ptr<Entity> &other : m_rooms)
    {
        if (other->get<CRoom>().type == RoomType::Rest && HasLink(other, room))
        {
            return true;
        }
    }
    return false;
}

static std::vector<EnemyId> PickEnemyGroup(std::mt19937 &rng, int floor)
{
    if (floor < mapFloors / 2)
    {
        return {EnemyId::Slime};
    }
    std::uniform_int_distribution<int> pickGroup(0, 1);
    if (pickGroup(rng) == 0)
    {
        return {EnemyId::Slime, EnemyId::Slime};
    }
    return {EnemyId::Guardian};
}

void MapGenerator::assignRooms(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> pickType(0, 3);
    for (int floor = 0; floor < mapFloors; floor++)
    {
        for (const std::shared_ptr<Entity> &entity : m_rooms)
        {
            CRoom &room = entity->get<CRoom>();
            if (room.floor != floor || room.type == RoomType::Start || room.type == RoomType::Boss)
            {
                continue;
            }

            bool lastFloor = floor == mapFloors - 1;
            bool canRest = floor >= 2 && floor < mapFloors - 2 && !hasRestParent(entity);
            bool rolledRest = canRest && pickType(rng) == 0;
            room.type = lastFloor || rolledRest ? RoomType::Rest : RoomType::Combat;
            if (room.type == RoomType::Combat)
            {
                room.enemies = PickEnemyGroup(rng, floor);
            }
        }
    }
}

// Walks one path from the start room up to the last floor. Rooms are created where the
// path is the first to arrive; where another path already made a room, the paths merge.
void MapGenerator::addPath(RoomGrid &grid, std::mt19937 &rng, int firstColumn)
{
    int column = firstColumn;
    std::shared_ptr<Entity> previous = m_start;
    for (int floor = 0; floor < mapFloors; floor++)
    {
        if (floor > 0)
        {
            column = pickNextColumn(grid, rng, floor - 1, column);
        }
        if (!grid[floor][column])
        {
            grid[floor][column] = addRoom(RoomType::Combat, floor, column);
        }
        addLink(previous, grid[floor][column]);
        previous = grid[floor][column];
    }
}

// The boss sits above the grid; every room on the last floor leads to it.
void MapGenerator::addBoss(const RoomGrid &grid)
{
    std::shared_ptr<Entity> boss = addRoom(RoomType::Boss, mapFloors, 0);
    for (int column = 0; column < mapColumns; column++)
    {
        if (grid[mapFloors - 1][column])
        {
            addLink(grid[mapFloors - 1][column], boss);
        }
    }
    boss->get<CRoom>().enemies = {bossEnemy};
}

void MapGenerator::generateMap(std::uint32_t seed)
{
    std::mt19937 rng(seed);
    m_rooms.clear();

    // The start room sits below the grid and counts as already visited.
    m_start = addRoom(RoomType::Start, -1, 0);
    m_start->get<CRoom>().cleared = true;
    RoomGrid grid(mapFloors, std::vector<std::shared_ptr<Entity>>(mapColumns));

    std::uniform_int_distribution<int> pickColumn(0, mapColumns - 1);
    int firstColumn = -1;
    for (int path = 0; path < mapPaths; path++)
    {
        int column = pickColumn(rng);
        // The second path must begin in another column than the first, so the player
        // always has at least two rooms to choose from at the start.
        while (path == 1 && column == firstColumn)
        {
            column = pickColumn(rng);
        }
        if (path == 0)
        {
            firstColumn = column;
        }
        addPath(grid, rng, column);
    }

    addBoss(grid);
    assignRooms(rng);
}

std::shared_ptr<Entity> MapGenerator::generateValidMap(std::uint32_t seed)
{
    for (int attempt = 0; attempt < mapAttempts; attempt++)
    {
        std::uint32_t mapSeed = seed + static_cast<std::uint32_t>(attempt);
        generateMap(mapSeed);
        if (ValidateMap(m_rooms, m_start))
        {
            return m_start;
        }
        TraceLog(LOG_WARNING, "Map check failed for seed %u, trying again", static_cast<unsigned>(mapSeed));
        for (const std::shared_ptr<Entity> &room : m_rooms)
        {
            room->destroy();
        }
    }
    TraceLog(LOG_ERROR, "Could not generate a valid map, using seed %u anyway", static_cast<unsigned>(seed));
    generateMap(seed);
    return m_start;
}

// ---------------------------------------------------------------------------
// Checks

static std::shared_ptr<Entity> FindBoss(const EntityVec &rooms)
{
    std::shared_ptr<Entity> boss;
    for (const std::shared_ptr<Entity> &room : rooms)
    {
        if (room->get<CRoom>().type == RoomType::Boss)
        {
            boss = room;
        }
    }
    return boss;
}

// Every room except the boss leads somewhere, and every room except the start has a way in.
static bool EveryRoomIsLinked(const EntityVec &rooms, const std::shared_ptr<Entity> &start, const std::shared_ptr<Entity> &boss)
{
    std::map<std::size_t, int> incoming; // room id -> number of links arriving at it
    for (const std::shared_ptr<Entity> &room : rooms)
    {
        for (const std::shared_ptr<Entity> &next : room->get<CRoom>().next)
        {
            incoming[next->id()]++;
        }
    }

    for (const std::shared_ptr<Entity> &room : rooms)
    {
        bool deadEnd = room != boss && room->get<CRoom>().next.empty();
        bool noWayIn = room != start && incoming[room->id()] == 0;
        if (deadEnd || noWayIn)
        {
            return false;
        }
    }
    return true;
}

// Two links cross when they leave the same floor and swap sides on the way up.
static bool LinksCross(const CRoom &fromA, const CRoom &toA, const CRoom &fromB, const CRoom &toB)
{
    // The start and boss rooms are outside the grid, so their columns mean nothing.
    bool allOnGrid = IsOnGrid(fromA) && IsOnGrid(toA) && IsOnGrid(fromB) && IsOnGrid(toB);
    if (!allOnGrid || fromA.floor != fromB.floor)
    {
        return false;
    }
    return fromA.column < fromB.column && toA.column > toB.column;
}

// True when a link leaving room a crosses a link leaving room b.
static bool RoomLinksCross(const CRoom &a, const CRoom &b)
{
    for (const std::shared_ptr<Entity> &aNext : a.next)
    {
        for (const std::shared_ptr<Entity> &bNext : b.next)
        {
            if (LinksCross(a, aNext->get<CRoom>(), b, bNext->get<CRoom>()))
            {
                return true;
            }
        }
    }
    return false;
}

static bool AnyLinksCross(const EntityVec &rooms)
{
    for (const std::shared_ptr<Entity> &a : rooms)
    {
        for (const std::shared_ptr<Entity> &b : rooms)
        {
            if (RoomLinksCross(a->get<CRoom>(), b->get<CRoom>()))
            {
                return true;
            }
        }
    }
    return false;
}

// Follows the links from one room and reports whether they lead to the other.
static bool CanReach(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to)
{
    std::map<std::size_t, bool> visited; // room id -> already in the queue
    std::queue<std::shared_ptr<Entity>> queue;
    queue.push(from);
    visited[from->id()] = true;
    while (!queue.empty())
    {
        std::shared_ptr<Entity> room = queue.front();
        queue.pop();
        for (const std::shared_ptr<Entity> &next : room->get<CRoom>().next)
        {
            if (!visited[next->id()])
            {
                visited[next->id()] = true;
                queue.push(next);
            }
        }
    }
    return visited[to->id()];
}

bool ValidateMap(const EntityVec &rooms, const std::shared_ptr<Entity> &start)
{
    std::shared_ptr<Entity> boss = FindBoss(rooms);
    if (!start || !boss)
    {
        return false;
    }
    return EveryRoomIsLinked(rooms, start, boss) && !AnyLinksCross(rooms) && CanReach(start, boss);
}
