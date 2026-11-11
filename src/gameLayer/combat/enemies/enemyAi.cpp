#include "enemyAi.h"
#include "enemyDatabase.h"

// Below 30 percent
static bool IsLowHp(const Enemy &enemy, const EnemyDefinition &definition)
{
    return enemy.hp * 10 < definition.maxHp * 3;
}

Intent ChooseIntent(const Enemy &enemy, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);

    // Defend first
    if (IsLowHp(enemy, definition))
    {
        return Intent{IntentType::Defend, definition.block};
    }

    // Buff every third turn
    if (turn % 3 == 0)
    {
        return Intent{IntentType::Buff, definition.buff};
    }

    // Attack
    return Intent{IntentType::Attack, definition.damage + enemy.strength};
}
