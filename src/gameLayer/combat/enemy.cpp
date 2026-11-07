#include "enemy.h"
#include "enemyDatabase.h"

Enemy CreateEnemy(EnemyId id)
{
    Enemy enemy;
    enemy.id = id;
    enemy.hp = GetEnemyDefinition(id).maxHp;
    return enemy;
}

const char *GetIntentName(IntentType type)
{
    switch (type)
    {
    case IntentType::Attack:
        return "Attack";
    case IntentType::Defend:
        return "Defend";
    case IntentType::Buff:
        return "Buff";
    }
    return "Unknown";
}