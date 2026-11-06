#pragma once
#include "enemyDatabase.h"

enum class IntentType
{
    None,
    Attack,
    Defend,
    Buff,
};

class Intent
{
public:
    IntentType type = IntentType::None;
    int value = 0;
};

class Enemy
{
public:
    EnemyId id;
    int hp;
    int block = 0;
    int strength = 0;
    Intent intent;

    explicit Enemy(EnemyId enemyId)
    {
        id = enemyId;
        const EnemyDefinition &definition = GetEnemyDefinition(enemyId);
        hp = definition.maxHp;
    }
};
