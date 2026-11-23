#pragma once
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include "ecs/EntityManager.h"

inline constexpr int mapFloors = 7;
inline constexpr int mapColumns = 4;
inline constexpr int mapPaths = 3;

using RoomGrid = std::vector<std::vector<std::shared_ptr<Entity>>>;

// Builds the dungeon map as "room" entities: mapPaths paths climb from the start room to
// the boss, each step moving at most one column sideways without crossing another path.
class MapGenerator
{
    EntityManager &m_entityManager;
    EntityVec m_rooms;
    std::shared_ptr<Entity> m_start;

    std::shared_ptr<Entity> addRoom(RoomType type, int floor, int column);
    void addLink(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to);
    int pickNextColumn(const RoomGrid &grid, std::mt19937 &rng, int floor, int column) const;
    bool hasRestParent(const std::shared_ptr<Entity> &room) const;
    void assignRooms(std::mt19937 &rng);
    void generateMap(std::uint32_t seed);

public:
    explicit MapGenerator(EntityManager &entityManager);

    // Returns the start room. Retries with the next seeds if a map fails ValidateMap.
    std::shared_ptr<Entity> generateValidMap(std::uint32_t seed);
    const EntityVec &rooms() const { return m_rooms; }
};

bool IsOnGrid(const CRoom &room);
bool HasLink(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to);
// Every room leads somewhere and can be reached, no paths cross, and the boss is reachable.
bool ValidateMap(const EntityVec &rooms, const std::shared_ptr<Entity> &start);
