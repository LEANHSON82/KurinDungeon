#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "combat/enemies/enemyDefinition.h"

const int mapFloors = 7;
const int mapColumns = 4;
const int mapPaths = 3;

enum class RoomType
{
    Start,
    Combat,
    Rest,
    Boss,
};

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
    static DungeonMap Generate(std::uint32_t seed);

    bool CanVisit(int fromRoom, int toRoom) const;
    void MarkCleared(int roomId);

    bool IsValidRoom(int roomId) const;
    bool Validate() const;
    int GetRoomCount() const { return static_cast<int>(rooms.size()); }
    const Room &GetRoom(int roomId) const { return rooms[roomId]; }

private:
    int AddRoom(RoomType type, int floor, int column);
    void AddEdge(int fromRoom, int toRoom);
    bool HasEdge(int fromRoom, int toRoom) const;
    int PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int floor, int column) const;
    bool HasRestParent(int roomId) const;
    void AssignRooms(std::mt19937 &rng);

    std::vector<Room> rooms;
};
