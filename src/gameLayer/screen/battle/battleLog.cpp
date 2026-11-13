#include <raylib.h>
#include "battleLog.h"
#include "combat/enemies/enemyDatabase.h"

static const char *GetResultName(BattleResult result)
{
    switch (result)
    {
    case BattleResult::InProgress:
        return "InProgress";
    case BattleResult::Won:
        return "Won";
    case BattleResult::Lost:
        return "Lost";
    }

    return "Unknown";
}

void LogBattle(const Battle &battle, const char *label)
{
    const Deck &deck = battle.GetDeck();
    TraceLog(LOG_INFO,
             "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Draw %d | Hand %d | Discard %d",
             label, battle.GetTurn(), GetResultName(battle.GetResult()),
             battle.GetPlayer().hp, battle.GetPlayerBlock(), battle.GetEnergy(),
             static_cast<int>(deck.GetDrawPile().size()), static_cast<int>(deck.GetHand().size()),
             static_cast<int>(deck.GetDiscardPile().size()));

    const std::vector<Enemy> &enemies = battle.GetEnemies();
    for (int i = 0; i < static_cast<int>(enemies.size()); i++)
    {
        const Enemy &enemy = enemies[i];
        TraceLog(LOG_INFO, "    Enemy %d %s | HP %d Block %d Strength %d | Intent %s %d",
                 i, GetEnemyDefinition(enemy.id).name, enemy.hp, enemy.block, enemy.strength,
                 GetIntentName(enemy.intent.type), enemy.intent.value);
    }
}
