#include "core/systems/run/mapGenerator.h"
#include <algorithm>
#include <queue>
#include <random>
#include <unordered_map>
#include <vector>
#include "core/components/map.h"
#include "core/prefabs/prefabs.h"
#include "core/systems/run/mapRules.h"

using Grid = std::vector<std::vector<ecs::Entity>>;

namespace
{
struct MapBuilder
{
    ecs::Registry &registry;
    std::vector<ecs::Entity> rooms;

    ecs::Entity AddRoom(RoomType type, int floor, int column)
    {
        ecs::Entity room = SpawnRoom(registry, type, floor, column);
        rooms.push_back(room);
        return room;
    }

    void AddLink(ecs::Entity fromRoom, ecs::Entity toRoom)
    {
        if (!HasLink(registry, fromRoom, toRoom))
        {
            registry.Get<RoomLinks>(fromRoom).next.push_back(toRoom);
        }
    }

    int PickNextColumn(const Grid &grid, std::mt19937 &rng, int floor, int column) const
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
                ecs::Entity sideRoom = grid[floor][nextColumn];
                ecs::Entity crossTarget = grid[floor + 1][column];
                if (HasLink(registry, sideRoom, crossTarget))
                {
                    continue;
                }
            }

            choices.push_back(nextColumn);
        }

        std::uniform_int_distribution<int> pick(0, static_cast<int>(choices.size()) - 1);
        return choices[pick(rng)];
    }

    bool HasRestParent(ecs::Entity room) const
    {
        for (ecs::Entity other : rooms)
        {
            if (registry.Get<Room>(other).type == RoomType::Rest && HasLink(registry, other, room))
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

    void AssignRooms(std::mt19937 &rng)
    {
        std::uniform_int_distribution<int> pickType(0, 3);
        for (int floor = 0; floor < mapFloors; floor++)
        {
            for (ecs::Entity entity : rooms)
            {
                Room &room = registry.Get<Room>(entity);
                if (room.floor != floor || room.type == RoomType::Start || room.type == RoomType::Boss)
                {
                    continue;
                }

                bool lastFloor = floor == mapFloors - 1;
                bool canRest = floor >= 2 && floor < mapFloors - 2 && !HasRestParent(entity);
                bool rolledRest = canRest && pickType(rng) == 0;
                room.type = lastFloor || rolledRest ? RoomType::Rest : RoomType::Combat;

                if (room.type == RoomType::Combat)
                {
                    registry.Emplace<Encounter>(entity, PickEnemyGroup(rng, floor));
                }
            }
        }
    }
};
}

ecs::Entity GenerateMap(ecs::Registry &registry, std::uint32_t seed)
{
    std::mt19937 rng(seed);
    MapBuilder builder{registry, {}};

    ecs::Entity start = builder.AddRoom(RoomType::Start, -1, 0);
    registry.Emplace<Cleared>(start);

    Grid grid(mapFloors, std::vector<ecs::Entity>(mapColumns, ecs::nullEntity));

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

        ecs::Entity previous = start;
        for (int floor = 0; floor < mapFloors; floor++)
        {
            if (floor > 0)
            {
                column = builder.PickNextColumn(grid, rng, floor - 1, column);
            }
            if (grid[floor][column].IsNull())
            {
                grid[floor][column] = builder.AddRoom(RoomType::Combat, floor, column);
            }

            ecs::Entity room = grid[floor][column];
            builder.AddLink(previous, room);
            previous = room;
        }
    }

    ecs::Entity boss = builder.AddRoom(RoomType::Boss, mapFloors, 0);
    for (int column = 0; column < mapColumns; column++)
    {
        ecs::Entity room = grid[mapFloors - 1][column];
        if (!room.IsNull())
        {
            builder.AddLink(room, boss);
        }
    }

    builder.AssignRooms(rng);
    registry.Emplace<Encounter>(boss, std::vector<EnemyId>{EnemyId::Warden});
    return start;
}

static ecs::Entity FindRoomOfType(const ecs::Registry &registry, const std::vector<ecs::Entity> &rooms, RoomType type)
{
    for (ecs::Entity room : rooms)
    {
        if (registry.Get<Room>(room).type == type)
        {
            return room;
        }
    }
    return ecs::nullEntity;
}

static bool EveryRoomIsLinked(const ecs::Registry &registry, const std::vector<ecs::Entity> &rooms, ecs::Entity start, ecs::Entity boss)
{
    std::unordered_map<std::uint32_t, int> incoming;
    for (ecs::Entity room : rooms)
    {
        for (ecs::Entity next : registry.Get<RoomLinks>(room).next)
        {
            incoming[next.index]++;
        }
    }

    for (ecs::Entity room : rooms)
    {
        if (room != boss && registry.Get<RoomLinks>(room).next.empty())
        {
            return false;
        }
        if (room != start && incoming[room.index] == 0)
        {
            return false;
        }
    }
    return true;
}

static bool PathsCross(const ecs::Registry &registry, const std::vector<ecs::Entity> &rooms)
{
    for (ecs::Entity a : rooms)
    {
        const Room &aRoom = registry.Get<Room>(a);
        for (ecs::Entity aNext : registry.Get<RoomLinks>(a).next)
        {
            for (ecs::Entity b : rooms)
            {
                const Room &bRoom = registry.Get<Room>(b);
                for (ecs::Entity bNext : registry.Get<RoomLinks>(b).next)
                {
                    const Room &aTo = registry.Get<Room>(aNext);
                    const Room &bTo = registry.Get<Room>(bNext);
                    bool sameFloors = aRoom.IsOnGrid() && bRoom.IsOnGrid() &&
                                      aTo.IsOnGrid() && bTo.IsOnGrid() &&
                                      aRoom.floor == bRoom.floor;
                    if (sameFloors && aRoom.column < bRoom.column && aTo.column > bTo.column)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

static bool CanReach(const ecs::Registry &registry, ecs::Entity from, ecs::Entity to)
{
    std::unordered_map<std::uint32_t, bool> visited;
    std::queue<ecs::Entity> queue;
    queue.push(from);
    visited[from.index] = true;
    while (!queue.empty())
    {
        ecs::Entity room = queue.front();
        queue.pop();
        for (ecs::Entity next : registry.Get<RoomLinks>(room).next)
        {
            if (!visited[next.index])
            {
                visited[next.index] = true;
                queue.push(next);
            }
        }
    }
    return visited[to.index];
}

bool ValidateMap(const ecs::Registry &registry)
{
    std::vector<ecs::Entity> rooms = registry.View<Room>().Collect();
    ecs::Entity start = FindRoomOfType(registry, rooms, RoomType::Start);
    ecs::Entity boss = FindRoomOfType(registry, rooms, RoomType::Boss);
    if (start.IsNull() || boss.IsNull())
    {
        return false;
    }

    return EveryRoomIsLinked(registry, rooms, start, boss) &&
           !PathsCross(registry, rooms) &&
           CanReach(registry, start, boss);
}
