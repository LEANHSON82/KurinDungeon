#include <raylib.h>
#include "battleScreen.h"
#include "cards/cardDatabase.h"
#include "combat/enemies/enemyDatabase.h"
#include "gameConfig.h"

// Layout
static const int fontSize = 25;
static const int titleSize = 35;
static const int lineHeight = 40;
static const int padding = 30;
static const int panelTop = 120;
static const int panelWidth = 800;
static const int panelHeight = 760;
static const int playerX = 100;
static const int enemyX = screenWidth - 100 - panelWidth;

// Draw helpers
static int Count(const std::vector<CardId> &cards)
{
    return static_cast<int>(cards.size());
}

static void DrawTextLine(const char *text, int x, int &y, Color color)
{
    DrawText(text, x, y, fontSize, color);
    y += lineHeight;
}

static void DrawCenteredText(const char *text, int y, int size, Color color)
{
    DrawText(text, (screenWidth - MeasureText(text, size)) / 2, y, size, color);
}

static int DrawSideTitle(int x, const char *title, Color color)
{
    DrawText(title, x + padding, panelTop + padding, titleSize, color);
    return panelTop + padding + titleSize + lineHeight;
}

// Result name
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

// Debug log
static void LogBattle(const Battle &battle, const char *label)
{
    const Deck &deck = battle.GetDeck();
    const Enemy &enemy = battle.GetEnemy();
    TraceLog(LOG_INFO,
             "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Enemy HP %d Block %d Strength %d | Intent %s %d | Draw %d | Hand %d | Discard %d",
             label, battle.GetTurn(), GetResultName(battle.GetResult()),
             battle.GetPlayer().hp, battle.GetPlayerBlock(), battle.GetEnergy(),
             enemy.hp, enemy.block, enemy.strength,
             GetIntentName(enemy.intent.type), enemy.intent.value,
             Count(deck.GetDrawPile()), Count(deck.GetHand()), Count(deck.GetDiscardPile()));
}

// Update
ScreenAction UpdateBattleScreen(Battle &battle)
{
    // Leave finished battle
    if (battle.GetResult() != BattleResult::InProgress)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            return ScreenAction::LeaveBattle;
        }
        return ScreenAction::None;
    }

    // End turn
    if (IsKeyPressed(KEY_E))
    {
        LogBattle(battle, "Before EndTurn");
        battle.EndPlayerTurn();
        LogBattle(battle, "After EndTurn");
        return ScreenAction::None;
    }

    // Play card
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

// Player panel
static void DrawPlayerSide(const Battle &battle)
{
    const PlayerState &player = battle.GetPlayer();
    const Deck &deck = battle.GetDeck();
    const int x = playerX + padding;

    int y = DrawSideTitle(playerX, "PLAYER", DARKBLUE);
    DrawTextLine(TextFormat("HP: %d / %d", player.hp, player.maxHp), x, y, BLACK);
    DrawTextLine(TextFormat("Block: %d", battle.GetPlayerBlock()), x, y, BLACK);
    DrawTextLine(TextFormat("Energy: %d / %d", battle.GetEnergy(), Battle::startingEnergy), x, y, BLACK);
    DrawTextLine(TextFormat("Draw: %d | Hand: %d | Discard: %d",
                            Count(deck.GetDrawPile()), Count(deck.GetHand()), Count(deck.GetDiscardPile())),
                 x, y, BLACK);
    y += lineHeight;

    DrawTextLine("Hand:", x, y, DARKBLUE);
    const std::vector<CardId> &hand = deck.GetHand();
    for (int i = 0; i < Count(hand); i++)
    {
        const Card &card = GetCard(hand[i]);
        Color color = battle.CanPlayCard(i) ? DARKGREEN : GRAY;
        DrawTextLine(TextFormat("%d: %s | Cost %d | %s", i + 1, card.name, card.cost, DescribeEffects(card).c_str()),
                     x, y, color);
    }
}

// Enemy panel
static void DrawEnemySide(const Battle &battle)
{
    const Enemy &enemy = battle.GetEnemy();
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);
    const int x = enemyX + padding;

    int y = DrawSideTitle(enemyX, definition.name, MAROON);
    DrawTextLine(TextFormat("HP: %d / %d", enemy.hp, definition.maxHp), x, y, BLACK);
    DrawTextLine(TextFormat("Block: %d", enemy.block), x, y, BLACK);
    DrawTextLine(TextFormat("Strength: %d", enemy.strength), x, y, BLACK);
    y += lineHeight;
    DrawTextLine(TextFormat("Intent: %s %d", GetIntentName(enemy.intent.type), enemy.intent.value), x, y, MAROON);
}

// Draw
void DrawBattleScreen(const Battle &battle)
{
    DrawCenteredText(TextFormat("BATTLE - Turn %d", battle.GetTurn()), 40, 40, RED);
    DrawPlayerSide(battle);
    DrawEnemySide(battle);

    const int y = panelTop + panelHeight + 50;
    switch (battle.GetResult())
    {
    case BattleResult::InProgress:
        DrawCenteredText("1-5: Play card | E: End turn", y, fontSize, GRAY);
        break;
    case BattleResult::Won:
        DrawCenteredText("WON! Press ENTER to continue", y, titleSize, DARKGREEN);
        break;
    case BattleResult::Lost:
        DrawCenteredText("LOST! Press ENTER to continue", y, titleSize, RED);
        break;
    }
}
