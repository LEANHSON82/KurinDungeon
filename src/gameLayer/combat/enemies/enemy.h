#pragma once
#include "enemyDefinition.h"

// Intent types
enum class IntentType
{
    Attack,
    Defend,
    Buff,
};

// Announced action
struct Intent
{
    IntentType type = IntentType::Attack;
    int value = 0;
};

// Battle state
struct Enemy
{
    EnemyId id = EnemyId::Slime;
    int hp = 0;
    int block = 0;
    int strength = 0;
    Intent intent;
};

// Helpers
Enemy CreateEnemy(EnemyId id);
const char *GetIntentName(IntentType type);
