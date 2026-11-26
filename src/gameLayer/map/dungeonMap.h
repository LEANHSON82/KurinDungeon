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
    int floor = 0;
    int column = 0;
    std::vector<int> next;
    std::vector<EnemyId> enemies;
    bool cleared = false;

    bool IsOnGrid() const { return floor >= 0 && floor < mapFloors; }
};

class DungeonMap
{
public:
    static DungeonMap Generate(std::uint32_t seed);

    bool CanVisit(int fromRoom, int toRoom) const;
    void MarkCleared(int roomId);

    bool IsValidRoom(int roomId) const;
    bool HasEdge(int fromRoom, int toRoom) const;
    bool Validate() const;
    int GetStartRoom() const { return 0; }
    int GetRoomCount() const { return static_cast<int>(rooms.size()); }
    const Room &GetRoom(int roomId) const { return rooms[roomId]; }

private:
    static DungeonMap GenerateOnce(std::uint32_t seed);

    int AddRoom(RoomType type, int floor, int column);
    void AddEdge(int fromRoom, int toRoom);
    int PickNextColumn(const std::vector<std::vector<int>> &grid, std::mt19937 &rng, int fromFloor, int column) const;
    void AddPath(std::vector<std::vector<int>> &grid, std::mt19937 &rng, int firstColumn);
    void AddBoss(const std::vector<std::vector<int>> &grid);
    bool HasRestParent(int roomId) const;
    void AssignRooms(std::mt19937 &rng);

    int FindBoss() const;
    bool EveryRoomIsLinked(int bossRoom) const;
    bool EdgesCross(int fromA, int toA, int fromB, int toB) const;
    bool AnyEdgesCross() const;
    bool CanReach(int fromRoom, int toRoom) const;

    std::vector<Room> rooms;
};
