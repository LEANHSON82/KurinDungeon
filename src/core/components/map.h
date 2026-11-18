#pragma once
#include <vector>
#include "core/data/enemies.h"
#include "ecs/entity.h"

inline constexpr int mapFloors = 7;
inline constexpr int mapColumns = 4;
inline constexpr int mapPaths = 3;

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

    bool IsOnGrid() const { return floor >= 0 && floor < mapFloors; }
};

struct RoomLinks
{
    std::vector<ecs::Entity> next;
};

struct Encounter
{
    std::vector<EnemyId> enemies;
};

struct Cleared
{
};
