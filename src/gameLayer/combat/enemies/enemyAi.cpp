#include "enemyAi.h"
#include "enemyDatabase.h"

const int bossAbilityHpPercent = 50;
const int bossAbilityBlockMultiplier = 2;
const int lowHpDefendPercent = 30;
const int intentCycleTurns = 3;
const int debuffTurnInCycle = 2;
const int buffTurnInCycle = 0;

static bool IsBelowPercent(const Enemy &enemy, const EnemyDefinition &definition, int percent)
{
    return enemy.hp * 100 < definition.maxHp * percent;
}

Intent ChooseIntent(const Enemy &enemy, int turn)
{
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);

    if (definition.bossAbility && !enemy.usedBossAction && IsBelowPercent(enemy, definition, bossAbilityHpPercent))
    {
        return Intent{IntentType::Defend, definition.block * bossAbilityBlockMultiplier, true};
    }

    if (IsBelowPercent(enemy, definition, lowHpDefendPercent))
    {
        return Intent{IntentType::Defend, definition.block};
    }

    int turnInCycle = turn % intentCycleTurns;
    if (turnInCycle == buffTurnInCycle)
    {
        return Intent{IntentType::Buff, definition.buff};
    }
    if (definition.debuffAmount > 0 && turnInCycle == debuffTurnInCycle)
    {
        return Intent{IntentType::Debuff, definition.debuffAmount};
    }

    return Intent{IntentType::Attack, definition.damage};
}
