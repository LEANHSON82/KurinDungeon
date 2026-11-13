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
    bool bossAction = false;
};

struct Enemy
{
    EnemyId id = EnemyId::Slime;
    int hp = 0;
    int block = 0;
    int strength = 0;
    bool usedBossAction = false;
    Intent intent;
};

Enemy CreateEnemy(EnemyId id);
const char *GetIntentName(IntentType type);
