#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "combat/enemies/enemyDefinition.h"

// Map size
const int mapFloors = 7;
const int mapColumns = 4;
const int mapPaths = 3;

// Room types
enum class RoomType
{
    Start,
    Combat,
    Rest,
    Boss,
};

// Room data
struct Room
{
    RoomType type = RoomType::Start;
    std::vector<int> next;
    bool cleared = false;
    std::vector<EnemyId> enemies;
    int floor = 0;
    int column = 0;

    bool IsOnGrid() const { return floor >= 0 && floor < mapFloors; }
};

class DungeonMap
{
public:
    // Setup
    static DungeonMap Generate(std::uint32_t seed);

    // Actions
    bool CanVisit(int fromRoom, int toRoom) const;
    void MarkCleared(int roomId);

    // Read only
    bool IsValidRoom(int roomId) const;
    bool Validate() const;
    int GetRoomCount() const { return static_cast<int>(rooms.size()); }
    const Room &GetRoom(int roomId) const { return rooms[roomId]; }

private:
    // Build steps
    int AddRoom(RoomType type, int floor, int column);
    void AddEdge(int fromRoom, int toRoom);
    bool HasEdge(int fromRoom, int toRoom) const;
    int PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int floor, int column) const;
    bool HasRestParent(int roomId) const;
    void AssignRooms(std::mt19937 &rng);

    std::vector<Room> rooms;
};
