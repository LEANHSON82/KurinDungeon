#pragma once

enum class EnemyId
{
    Slime,
};

class EnemyDefinition
{
public:
    const char *name = "";
    int maxHp = 0;
    int damage = 0;
};
