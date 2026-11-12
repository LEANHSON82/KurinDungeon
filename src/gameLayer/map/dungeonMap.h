#pragma once
#include <vector>
#include "combat/enemies/enemyDefinition.h"

// Room types
enum class RoomType
{
    Start,
    Combat,
};

// Room data
struct Room
{
    RoomType type = RoomType::Start;
    std::vector<int> next;
    bool cleared = false;
    EnemyId enemy = EnemyId::Slime;
};

class DungeonMap
{
public:
    // Setup
    static DungeonMap CreateFixed();

    // Actions
    bool CanVisit(int fromRoom, int toRoom) const;
    void MarkCleared(int roomId);

    // Read only
    bool IsValidRoom(int roomId) const;
    const Room &GetRoom(int roomId) const { return rooms[roomId]; }

private:
    std::vector<Room> rooms;
};
