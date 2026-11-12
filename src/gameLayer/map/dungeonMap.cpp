#include <algorithm>
#include "dungeonMap.h"

// Start -> room -> room
DungeonMap DungeonMap::CreateFixed()
{
    DungeonMap map;
    map.rooms = {
        {RoomType::Start, {1}, true},
        {RoomType::Combat, {2}, false, EnemyId::Slime},
        {RoomType::Combat, {}, false, EnemyId::Guardian},
    };
    return map;
}

// Validate room
bool DungeonMap::IsValidRoom(int roomId) const
{
    return roomId >= 0 && roomId < static_cast<int>(rooms.size());
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

    const std::vector<int> &next = rooms[fromRoom].next;
    return std::find(next.begin(), next.end(), toRoom) != next.end();
}

// Clear room
void DungeonMap::MarkCleared(int roomId)
{
    if (IsValidRoom(roomId))
    {
        rooms[roomId].cleared = true;
    }
}
