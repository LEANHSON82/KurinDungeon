#include "enemyDatabase.h"

const EnemyDefinition &GetEnemyDefinition(EnemyId id)
{
    static const EnemyDefinition enemies[enemyTypeCount]{
        {"Slime", 30, 6, 5, 2, false},
        {"Guardian", 48, 8, 8, 3, false, StatusType::Vulnerable, 2},
        {"Brute", 72, 11, 9, 3, false, StatusType::Weak, 2},
        {"Axeman", 64, 14, 6, 2, false, StatusType::Vulnerable, 2},
        {"Warden", 90, 10, 10, 3, true, StatusType::Weak, 2},
    };
    return enemies[static_cast<int>(id)];
}
