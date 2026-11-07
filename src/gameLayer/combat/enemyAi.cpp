#include "enemyAi.h"
#include "enemyDatabase.h"

Intent ChooseIntent(const Enemy &enemy)
{
    return Intent{IntentType::Attack, GetEnemyDefinition(enemy.id).damage};
}
