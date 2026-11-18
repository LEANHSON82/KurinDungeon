#pragma once
#include "core/data/enemies.h"

struct Player
{
};

struct Enemy
{
    EnemyId id = EnemyId::Slime;
    int slot = 0;
};

struct Health
{
    int hp = 0;
    int maxHp = 0;
};

struct Block
{
    int value = 0;
};

struct Strength
{
    int value = 0;
};

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

struct BossAbility
{
    bool used = false;
};

struct Dead
{
};

// Destroyed when the battle ends.
struct BattleScoped
{
};

// Destroyed when a new run starts.
struct RunScoped
{
};
