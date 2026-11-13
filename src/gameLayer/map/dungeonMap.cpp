#include <algorithm>
#include <queue>
#include "dungeonMap.h"

// Generate map
DungeonMap DungeonMap::Generate(std::uint32_t seed)
{
    std::mt19937 rng(seed);
    DungeonMap map;

    // Start room
    int startId = map.AddRoom(RoomType::Start, -1, 0);
    map.rooms[startId].cleared = true;

    // Empty grid
    std::vector<std::vector<int>> grid(mapFloors, std::vector<int>(mapColumns, -1));

    // Paths
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

    // Boss room
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

// Add room
int DungeonMap::AddRoom(RoomType type, int floor, int column)
{
    Room room;
    room.type = type;
    room.floor = floor;
    room.column = column;
    rooms.push_back(room);
    return static_cast<int>(rooms.size()) - 1;
}

// Add edge once
void DungeonMap::AddEdge(int fromRoom, int toRoom)
{
    if (!HasEdge(fromRoom, toRoom))
    {
        rooms[fromRoom].next.push_back(toRoom);
    }
}

// Check edge
bool DungeonMap::HasEdge(int fromRoom, int toRoom) const
{
    if (fromRoom == -1 || toRoom == -1)
    {
        return false;
    }

    const std::vector<int> &next = rooms[fromRoom].next;
    return std::find(next.begin(), next.end(), toRoom) != next.end();
}

// Next column without crossing
int DungeonMap::PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int floor, int column) const
{
    std::vector<int> choices;
    for (int nextColumn = column - 1; nextColumn <= column + 1; nextColumn++)
    {
        if (nextColumn < 0 || nextColumn >= mapColumns)
        {
            continue;
        }

        // Diagonal edges must not cross
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

// Parent is rest
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

// Room types and enemies
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

            // Rest rules
            bool lastFloor = floor == mapFloors - 1;
            bool canRest = floor >= 2 && floor < mapFloors - 2 && !HasRestParent(id);
            bool rolledRest = canRest && pickType(rng) == 0;
            room.type = lastFloor || rolledRest ? RoomType::Rest : RoomType::Combat;

            // Enemy group
            room.enemy = floor < mapFloors / 2 ? EnemyId::Slime : EnemyId::Guardian;
        }
    }

    rooms.back().enemy = EnemyId::Warden;
}

// Graph check
bool DungeonMap::Validate() const
{
    int roomCount = GetRoomCount();
    int bossId = roomCount - 1;

    // Incoming and outgoing edges
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

    // Crossing edges
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

    // BFS from start
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

// Validate room
bool DungeonMap::IsValidRoom(int roomId) const
{
    return roomId >= 0 && roomId < GetRoomCount();
}

// Movement rule
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

// Clear room
void DungeonMap::MarkCleared(int roomId)
{
    if (IsValidRoom(roomId))
    {
        rooms[roomId].cleared = true;
    }
}
