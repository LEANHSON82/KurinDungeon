#pragma once

// Enemy IDs
enum class EnemyId
{
    Slime,
    Guardian,
    Warden,
};

// Base stats
struct EnemyDefinition
{
    const char *name = "";
    int maxHp = 0;
    int damage = 0;
    int block = 0;
    int buff = 0;
};
