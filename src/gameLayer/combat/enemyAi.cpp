#include "enemyAi.h"
#include "enemyDatabase.h"

static bool IsLowHp(const Enemy &enemy, const EnemyDefinition &definition)
{
    return enemy.hp * 10 < definition.maxHp * 3;
}

Intent ChooseIntent(const Enemy &enemy, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);

    if (IsLowHp(enemy, definition))
    {
        return Intent{IntentType::Defend, definition.block};
    }
    if (turn % 3 == 0)
    {
        return Intent{IntentType::Buff, definition.buff};
    }
    return Intent{IntentType::Attack, definition.damage + enemy.strength};
}
