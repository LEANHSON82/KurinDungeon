#include "dungeonMap.h"
#include <algorithm>
#include <queue>
#include <raylib.h>

const int mapAttempts = 16;

DungeonMap DungeonMap::Generate(std::uint32_t seed)
{
    for (int attempt = 0; attempt < mapAttempts; attempt++)
    {
        std::uint32_t mapSeed = seed + static_cast<std::uint32_t>(attempt);
        DungeonMap map = GenerateOnce(mapSeed);
        if (map.Validate())
        {
            return map;
        }
        TraceLog(LOG_WARNING, "Map check failed for seed %u, trying again", static_cast<unsigned>(mapSeed));
    }

    TraceLog(LOG_ERROR, "Could not generate a valid map, using seed %u anyway", static_cast<unsigned>(seed));
    return GenerateOnce(seed);
}

DungeonMap DungeonMap::GenerateOnce(std::uint32_t seed)
{
    DungeonMap map;
    std::mt19937 rng(seed);

    int start = map.AddRoom(RoomType::Start, -1, 0);
    map.rooms[start].cleared = true;
    std::vector<std::vector<int>> grid(mapFloors, std::vector<int>(mapColumns, -1));

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
        map.AddPath(grid, rng, column);
    }

    map.AddBoss(grid);
    map.AssignRooms(rng);
    return map;
}

bool DungeonMap::IsValidRoom(int roomId) const
{
    return roomId >= 0 && roomId < GetRoomCount();
}

bool DungeonMap::HasEdge(int fromRoom, int toRoom) const
{
    if (!IsValidRoom(fromRoom) || !IsValidRoom(toRoom))
    {
        return false;
    }
    const std::vector<int> &next = rooms[fromRoom].next;
    return std::find(next.begin(), next.end(), toRoom) != next.end();
}

bool DungeonMap::CanVisit(int fromRoom, int toRoom) const
{
    if (!IsValidRoom(fromRoom) || !IsValidRoom(toRoom))
    {
        return false;
    }
    if (!rooms[fromRoom].cleared || rooms[toRoom].cleared)
    {
        return false;
    }
    return HasEdge(fromRoom, toRoom);
}

void DungeonMap::MarkCleared(int roomId)
{
    if (IsValidRoom(roomId))
    {
        rooms[roomId].cleared = true;
    }
}

int DungeonMap::AddRoom(RoomType type, int floor, int column)
{
    Room room;
    room.type = type;
    room.floor = floor;
    room.column = column;
    rooms.push_back(room);
    return GetRoomCount() - 1;
}

void DungeonMap::AddEdge(int fromRoom, int toRoom)
{
    if (!HasEdge(fromRoom, toRoom))
    {
        rooms[fromRoom].next.push_back(toRoom);
    }
}

int DungeonMap::PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int fromFloor, int column) const
{
    std::vector<int> choices;
    for (int nextColumn = column - 1; nextColumn <= column + 1; nextColumn++)
    {
        if (nextColumn < 0 || nextColumn >= mapColumns)
        {
            continue;
        }
        if (nextColumn != column && HasEdge(grid[fromFloor][nextColumn], grid[fromFloor + 1][column]))
        {
            continue;
        }
        choices.push_back(nextColumn);
    }

    std::uniform_int_distribution<int> pick(0, static_cast<int>(choices.size()) - 1);
    return choices[pick(rng)];
}

void DungeonMap::AddPath(std::vector<std::vector<int>> &grid, std::mt19937 &rng, int firstColumn)
{
    int column = firstColumn;
    int previous = GetStartRoom();
    for (int floor = 0; floor < mapFloors; floor++)
    {
        if (floor > 0)
        {
            column = PickNextColumn(grid, rng, floor - 1, column);
        }
        if (grid[floor][column] == -1)
        {
            grid[floor][column] = AddRoom(RoomType::Combat, floor, column);
        }
        AddEdge(previous, grid[floor][column]);
        previous = grid[floor][column];
    }
}

void DungeonMap::AddBoss(const std::vector<std::vector<int>> &grid)
{
    int boss = AddRoom(RoomType::Boss, mapFloors, 0);
    for (int column = 0; column < mapColumns; column++)
    {
        if (grid[mapFloors - 1][column] != -1)
        {
            AddEdge(grid[mapFloors - 1][column], boss);
        }
    }
    rooms[boss].enemies = {bossEnemy};
}

bool DungeonMap::HasRestParent(int roomId) const
{
    for (int id = 0; id < GetRoomCount(); id++)
    {
        if (rooms[id].type == RoomType::Rest && HasEdge(id, roomId))
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

void DungeonMap::AssignRooms(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> pickType(0, 3);
    for (int floor = 0; floor < mapFloors; floor++)
    {
        for (int id = 0; id < GetRoomCount(); id++)
        {
            Room &room = rooms[id];
            if (room.floor != floor || room.type == RoomType::Start || room.type == RoomType::Boss)
            {
                continue;
            }

            bool lastFloor = floor == mapFloors - 1;
            bool canRest = floor >= 2 && floor < mapFloors - 2 && !HasRestParent(id);
            bool rolledRest = canRest && pickType(rng) == 0;
            room.type = lastFloor || rolledRest ? RoomType::Rest : RoomType::Combat;
            if (room.type == RoomType::Combat)
            {
                room.enemies = PickEnemyGroup(rng, floor);
            }
        }
    }
}

int DungeonMap::FindBoss() const
{
    int boss = -1;
    for (int id = 0; id < GetRoomCount(); id++)
    {
        if (rooms[id].type == RoomType::Boss)
        {
            boss = id;
        }
    }
    return boss;
}

bool DungeonMap::EveryRoomIsLinked(int bossRoom) const
{
    std::vector<int> incoming(rooms.size(), 0);
    for (const Room &room : rooms)
    {
        for (int next : room.next)
        {
            incoming[next]++;
        }
    }

    for (int id = 0; id < GetRoomCount(); id++)
    {
        bool deadEnd = id != bossRoom && rooms[id].next.empty();
        bool noWayIn = id != GetStartRoom() && incoming[id] == 0;
        if (deadEnd || noWayIn)
        {
            return false;
        }
    }
    return true;
}

bool DungeonMap::EdgesCross(int fromA, int toA, int fromB, int toB) const
{
    const Room &a = rooms[fromA];
    const Room &aNext = rooms[toA];
    const Room &b = rooms[fromB];
    const Room &bNext = rooms[toB];
    bool allOnGrid = a.IsOnGrid() && aNext.IsOnGrid() && b.IsOnGrid() && bNext.IsOnGrid();
    if (!allOnGrid || a.floor != b.floor)
    {
        return false;
    }
    return a.column < b.column && aNext.column > bNext.column;
}

bool DungeonMap::AnyEdgesCross() const
{
    for (int a = 0; a < GetRoomCount(); a++)
    {
        for (int b = 0; b < GetRoomCount(); b++)
        {
            for (int aNext : rooms[a].next)
            {
                for (int bNext : rooms[b].next)
                {
                    if (EdgesCross(a, aNext, b, bNext))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool DungeonMap::CanReach(int fromRoom, int toRoom) const
{
    std::vector<bool> visited(rooms.size(), false);
    std::queue<int> queue;
    queue.push(fromRoom);
    visited[fromRoom] = true;
    while (!queue.empty())
    {
        int room = queue.front();
        queue.pop();
        for (int next : rooms[room].next)
        {
            if (!visited[next])
            {
                visited[next] = true;
                queue.push(next);
            }
        }
    }
    return visited[toRoom];
}

bool DungeonMap::Validate() const
{
    int boss = FindBoss();
    if (rooms.empty() || boss == -1)
    {
        return false;
    }
    return EveryRoomIsLinked(boss) && !AnyEdgesCross() && CanReach(GetStartRoom(), boss);
}
