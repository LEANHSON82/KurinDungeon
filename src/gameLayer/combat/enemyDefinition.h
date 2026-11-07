#pragma once

enum class EnemyId
{
    Slime,
};

struct EnemyDefinition
{
    const char *name = "";
    int maxHp = 0;
    int damage = 0;
};
