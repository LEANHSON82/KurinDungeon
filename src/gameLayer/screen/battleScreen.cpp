#include <raylib.h>
#include "battleScreen.h"
#include "cards/cardDatabase.h"
#include "combat/enemyDatabase.h"

static const int left = 100;
static const int fontSize = 25;
static const int lineHeight = 40;

static int Count(const std::vector<CardId> &cards)
{
    return static_cast<int>(cards.size());
}

static void DrawTextLine(const char *text, int &y, Color color)
{
    DrawText(text, left, y, fontSize, color);
    y += lineHeight;
}

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

static void LogBattle(const Battle &battle, const char *label)
{
    const Deck &deck = battle.GetDeck();
    TraceLog(LOG_INFO,
             "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Enemy HP %d Block %d | Draw %d | Hand %d | Discard %d",
             label, battle.GetTurn(), GetResultName(battle.GetResult()),
             battle.GetPlayer().hp, battle.GetPlayerBlock(), battle.GetEnergy(),
             battle.GetEnemy().hp, battle.GetEnemy().block,
             Count(deck.GetDrawPile()), Count(deck.GetHand()), Count(deck.GetDiscardPile()));
}

ScreenAction UpdateBattleScreen(Battle &battle)
{
    if (battle.GetResult() != BattleResult::InProgress)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            return ScreenAction::LeaveBattle;
        }
        return ScreenAction::None;
    }

    if (IsKeyPressed(KEY_E))
    {
        LogBattle(battle, "Before EndTurn");
        battle.EndPlayerTurn();
        LogBattle(battle, "After EndTurn");
        return ScreenAction::None;
    }

    const int cardKeys[Battle::handSize]{KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE};
    for (int i = 0; i < Battle::handSize; i++)
    {
        if (IsKeyPressed(cardKeys[i]))
        {
            bool played = battle.PlayCard(i);
            LogBattle(battle, TextFormat("PlayCard(%d) %s", i, played ? "accepted" : "rejected"));
            break;
        }
    }

    return ScreenAction::None;
}

void DrawBattleScreen(const Battle &battle)
{
    const PlayerState &player = battle.GetPlayer();
    const Enemy &enemy = battle.GetEnemy();
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);
    const Deck &deck = battle.GetDeck();

    DrawText("BATTLE", left, 50, 40, RED);

    int y = 110;
    DrawTextLine(TextFormat("Turn: %d", battle.GetTurn()), y, BLACK);
    DrawTextLine(TextFormat("Player HP: %d / %d | Block: %d", player.hp, player.maxHp, battle.GetPlayerBlock()), y, BLACK);
    DrawTextLine(TextFormat("Energy: %d", battle.GetEnergy()), y, BLACK);
    DrawTextLine(TextFormat("%s HP: %d / %d | Block: %d", definition.name, enemy.hp, definition.maxHp, enemy.block), y, BLACK);
    DrawTextLine(TextFormat("Intent: %s %d", GetIntentName(enemy.intent.type), enemy.intent.value), y, BLACK);
    DrawTextLine(TextFormat("Draw: %d | Hand: %d | Discard: %d",
                            Count(deck.GetDrawPile()), Count(deck.GetHand()), Count(deck.GetDiscardPile())),
                 y, BLACK);
    y += lineHeight;

    const std::vector<CardId> &hand = deck.GetHand();
    for (int i = 0; i < Count(hand); i++)
    {
        const Card &card = GetCard(hand[i]);
        Color color = battle.CanPlayCard(i) ? DARKGREEN : GRAY;
        DrawTextLine(TextFormat("%d: %s | Cost %d | %s", i + 1, card.name, card.cost, DescribeEffects(card).c_str()), y, color);
    }
    y += lineHeight;

    switch (battle.GetResult())
    {
    case BattleResult::InProgress:
        DrawTextLine("1-5: Play card | E: End turn", y, GRAY);
        break;
    case BattleResult::Won:
        DrawTextLine("WON! Press ENTER to continue", y, DARKGREEN);
        break;
    case BattleResult::Lost:
        DrawTextLine("LOST! Press ENTER to continue", y, RED);
        break;
    }
}