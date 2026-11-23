#pragma once
#include "data/Statuses.h"

// Keep in the same order as the table in Enemies.cpp.
enum class EnemyId
{
    Slime,
    Guardian,
    Warden,
    Count,
};

inline constexpr int enemyCount = static_cast<int>(EnemyId::Count);
inline constexpr EnemyId bossEnemy = EnemyId::Warden;

struct EnemyDefinition
{
    const char *name = "";
    const char *sprites = ""; // animation prefix in assets.txt
    int maxHp = 0;
    int damage = 0;
    int block = 0;
    int buff = 0; // Strength gained by the buff intent
    bool bossAbility = false;
    StatusType debuff = StatusType::Weak; // put on the player by the debuff intent
    int debuffAmount = 0; // 0 = this enemy never debuffs
};

const EnemyDefinition &GetEnemyDefinition(EnemyId id);
