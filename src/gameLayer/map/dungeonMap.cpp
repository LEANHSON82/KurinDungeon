#include <algorithm>
#include <queue>
#include "dungeonMap.h"

DungeonMap DungeonMap::Generate(std::uint32_t seed)
{
    std::mt19937 rng(seed);
    DungeonMap map;

    int startId = map.AddRoom(RoomType::Start, -1, 0);
    map.rooms[startId].cleared = true;

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

        int previousId = startId;
        for (int floor = 0; floor < mapFloors; floor++)
        {
            if (floor > 0)
            {
                column = map.PickNextColumn(grid, rng, floor - 1, column);
            }
            if (grid[floor][column] == -1)
            {
                grid[floor][column] = map.AddRoom(RoomType::Combat, floor, column);
            }

            int roomId = grid[floor][column];
            map.AddEdge(previousId, roomId);
            previousId = roomId;
        }
    }

    int bossId = map.AddRoom(RoomType::Boss, mapFloors, 0);
    for (int column = 0; column < mapColumns; column++)
    {
        int roomId = grid[mapFloors - 1][column];
        if (roomId != -1)
        {
            map.AddEdge(roomId, bossId);
        }
    }

    map.AssignRooms(rng);
    return map;
}

int DungeonMap::AddRoom(RoomType type, int floor, int column)
{
    Room room;
    room.type = type;
    room.floor = floor;
    room.column = column;
    rooms.push_back(room);
    return static_cast<int>(rooms.size()) - 1;
}

void DungeonMap::AddEdge(int fromRoom, int toRoom)
{
    if (!HasEdge(fromRoom, toRoom))
    {
        rooms[fromRoom].next.push_back(toRoom);
    }
}

bool DungeonMap::HasEdge(int fromRoom, int toRoom) const
{
    if (fromRoom == -1 || toRoom == -1)
    {
        return false;
    }

    const std::vector<int> &next = rooms[fromRoom].next;
    return std::find(next.begin(), next.end(), toRoom) != next.end();
}

int DungeonMap::PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int floor, int column) const
{
    std::vector<int> choices;
    for (int nextColumn = column - 1; nextColumn <= column + 1; nextColumn++)
    {
        if (nextColumn < 0 || nextColumn >= mapColumns)
        {
            continue;
        }

        if (nextColumn != column)
        {
            int sideRoom = grid[floor][nextColumn];
            int crossTarget = grid[floor + 1][column];
            if (HasEdge(sideRoom, crossTarget))
            {
                continue;
            }
        }

        choices.push_back(nextColumn);
    }

    std::uniform_int_distribution<int> pick(0, static_cast<int>(choices.size()) - 1);
    return choices[pick(rng)];
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

    rooms.back().enemies = {EnemyId::Warden};
}

bool DungeonMap::Validate() const
{
    int roomCount = GetRoomCount();
    int bossId = roomCount - 1;

    std::vector<int> incoming(roomCount, 0);
    for (int id = 0; id < roomCount; id++)
    {
        if (id != bossId && rooms[id].next.empty())
        {
            return false;
        }
        for (int nextId : rooms[id].next)
        {
            incoming[nextId]++;
        }
    }
    for (int id = 1; id < roomCount; id++)
    {
        if (incoming[id] == 0)
        {
            return false;
        }
    }

    for (const Room &a : rooms)
    {
        for (int aNext : a.next)
        {
            for (const Room &b : rooms)
            {
                for (int bNext : b.next)
                {
                    const Room &aTo = rooms[aNext];
                    const Room &bTo = rooms[bNext];
                    bool sameFloors = a.IsOnGrid() && b.IsOnGrid() &&
                                      aTo.IsOnGrid() && bTo.IsOnGrid() &&
                                      a.floor == b.floor;
                    if (sameFloors && a.column < b.column && aTo.column > bTo.column)
                    {
                        return false;
                    }
                }
            }
        }
    }

    std::vector<bool> visited(roomCount, false);
    std::queue<int> queue;
    queue.push(0);
    visited[0] = true;
    while (!queue.empty())
    {
        int id = queue.front();
        queue.pop();
        for (int nextId : rooms[id].next)
        {
            if (!visited[nextId])
            {
                visited[nextId] = true;
                queue.push(nextId);
            }
        }
    }

    return visited[bossId];
}

bool DungeonMap::IsValidRoom(int roomId) const
{
    return roomId >= 0 && roomId < GetRoomCount();
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
