#include "core/data/enemies.h"
#include <stdexcept>

const EnemyDefinition &GetEnemyDefinition(EnemyId id)
{
    static const EnemyDefinition slime{"Slime", 30, 6, 5, 2, false};
    static const EnemyDefinition guardian{"Guardian", 48, 8, 8, 3, false};
    static const EnemyDefinition warden{"Warden", 90, 10, 10, 3, true};

    switch (id)
    {
    case EnemyId::Slime:
        return slime;
    case EnemyId::Guardian:
        return guardian;
    case EnemyId::Warden:
        return warden;
    }

    throw std::out_of_range("Invalid EnemyId");
}
