#pragma once
#include "combat/status.h"

enum class EnemyId
{
    Slime,
    Guardian,
    Brute,
    Axeman,
    Warden,
    Count,
};

const int enemyTypeCount = static_cast<int>(EnemyId::Count);
const EnemyId bossEnemy = EnemyId::Warden;

struct EnemyDefinition
{
    const char *name = "";
    int maxHp = 0;
    int damage = 0;
    int block = 0;
    int buff = 0;
    bool bossAbility = false;
    StatusType debuff = StatusType::Weak;
    int debuffAmount = 0;
};
