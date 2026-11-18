#include "core/data/enemies.h"
#include <cassert>
#include <iterator>

static const EnemyDefinition enemies[]{
    {"Slime", 30, 6, 5, 2, false},
    {"Guardian", 48, 8, 8, 3, false},
    {"Warden", 90, 10, 10, 3, true},
};
static_assert(std::size(enemies) == enemyCount, "Every EnemyId needs an entry in the enemies table");

const EnemyDefinition &GetEnemyDefinition(EnemyId id)
{
    int index = static_cast<int>(id);
    assert(index >= 0 && index < enemyCount && "Invalid EnemyId");
    return enemies[index];
}
