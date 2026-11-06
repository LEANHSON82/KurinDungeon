#include "enemyDatabase.h"
#include <stdexcept>

const EnemyDefinition &GetEnemyDefinition(EnemyId id)
{
    static const EnemyDefinition slime{"Slime", 30, 6};

    switch (id)
    {
    case EnemyId::Slime:
        return slime;
    }

    throw std::out_of_range("Invalid EnemyId");
}
