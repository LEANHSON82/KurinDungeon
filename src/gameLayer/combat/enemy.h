#pragma once
#include "enemyDefinition.h"

enum class IntentType
{
    Attack,
    Defend,
    Buff,
};

struct Intent
{
    IntentType type = IntentType::Attack;
    int value = 0;
};

struct Enemy
{
    EnemyId id = EnemyId::Slime;
    int hp = 0;
    int block = 0;
    int strength = 0;
    Intent intent;
};

Enemy CreateEnemy(EnemyId id);
const char *GetIntentName(IntentType type);
