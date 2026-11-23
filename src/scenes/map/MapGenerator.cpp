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

int MapGenerator::pickNextColumn(const RoomGrid &grid, std::mt19937 &rng, int floor, int column) const
{
    std::vector<int> choices;
    for (int nextColumn = column - 1; nextColumn <= column + 1; nextColumn++)
    {
        if (nextColumn < 0 || nextColumn >= mapColumns)
        {
            continue;
        }
        // Moving sideways must not cross a link that already goes the other way.
        if (nextColumn != column && HasLink(grid[floor][nextColumn], grid[floor + 1][column]))
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

void MapGenerator::generateMap(std::uint32_t seed)
{
    std::mt19937 rng(seed);
    m_rooms.clear();

    m_start = addRoom(RoomType::Start, -1, 0);
    m_start->get<CRoom>().cleared = true;
    RoomGrid grid(mapFloors, std::vector<std::shared_ptr<Entity>>(mapColumns));

    std::uniform_int_distribution<int> pickColumn(0, mapColumns - 1);
    int firstColumn = -1;
    for (int path = 0; path < mapPaths; path++)
    {
        int column = pickColumn(rng);
        while (path == 1 && column == firstColumn)
        {
            column = pickColumn(rng);
        }
        if (path == 0)
        {
            firstColumn = column;
        }

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

    std::shared_ptr<Entity> boss = addRoom(RoomType::Boss, mapFloors, 0);
    for (int column = 0; column < mapColumns; column++)
    {
        if (grid[mapFloors - 1][column])
        {
            addLink(grid[mapFloors - 1][column], boss);
        }
    }

    assignRooms(rng);
    boss->get<CRoom>().enemies = {bossEnemy};
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

bool ValidateMap(const EntityVec &rooms, const std::shared_ptr<Entity> &start)
{
    std::shared_ptr<Entity> boss;
    std::map<std::size_t, int> incoming;
    for (const std::shared_ptr<Entity> &room : rooms)
    {
        if (room->get<CRoom>().type == RoomType::Boss)
        {
            boss = room;
        }
        for (const std::shared_ptr<Entity> &next : room->get<CRoom>().next)
        {
            incoming[next->id()]++;
        }
    }
    if (!start || !boss)
    {
        return false;
    }

    // Every room leads somewhere and can be reached.
    for (const std::shared_ptr<Entity> &room : rooms)
    {
        if (room != boss && room->get<CRoom>().next.empty())
        {
            return false;
        }
        if (room != start && incoming[room->id()] == 0)
        {
            return false;
        }
    }

    // No two links between the same floors cross each other.
    for (const std::shared_ptr<Entity> &a : rooms)
    {
        const CRoom &aRoom = a->get<CRoom>();
        for (const std::shared_ptr<Entity> &aNext : aRoom.next)
        {
            for (const std::shared_ptr<Entity> &b : rooms)
            {
                const CRoom &bRoom = b->get<CRoom>();
                for (const std::shared_ptr<Entity> &bNext : bRoom.next)
                {
                    const CRoom &aTo = aNext->get<CRoom>();
                    const CRoom &bTo = bNext->get<CRoom>();
                    bool sameFloors = IsOnGrid(aRoom) && IsOnGrid(bRoom) && IsOnGrid(aTo) && IsOnGrid(bTo) && aRoom.floor == bRoom.floor;
                    if (sameFloors && aRoom.column < bRoom.column && aTo.column > bTo.column)
                    {
                        return false;
                    }
                }
            }
        }
    }

    // The boss can be reached from the start.
    std::map<std::size_t, bool> visited;
    std::queue<std::shared_ptr<Entity>> queue;
    queue.push(start);
    visited[start->id()] = true;
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
    return visited[boss->id()];
}
