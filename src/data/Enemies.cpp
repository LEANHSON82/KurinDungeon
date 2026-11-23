#include "data/Enemies.h"
#include <cassert>
#include <iterator>

// Columns: name, sprites, maxHp, damage, block, buff, bossAbility, debuff, debuffAmount.
// The last two are left out for an enemy that never debuffs.
static const EnemyDefinition enemies[]{
    {"Slime", "Slime", 30, 6, 5, 2, false},
    {"Guardian", "Orc", 48, 8, 8, 3, false, StatusType::Vulnerable, 2},
    {"Warden", "Necromancer", 90, 10, 10, 3, true, StatusType::Weak, 2},
};
static_assert(std::size(enemies) == enemyCount, "Every EnemyId needs an entry in the enemies table");

const EnemyDefinition &GetEnemyDefinition(EnemyId id)
{
    int index = static_cast<int>(id);
    assert(index >= 0 && index < enemyCount && "Invalid EnemyId");
    return enemies[index];
}
