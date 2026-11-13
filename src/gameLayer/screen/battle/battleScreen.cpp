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
    TraceLog(LOG_INFO,
             "%s | Turn %d | %s | Player HP %d Block %d | Energy %d | Draw %d | Hand %d | Discard %d",
             label, battle.GetTurn(), GetResultName(battle.GetResult()),
             battle.GetPlayer().hp, battle.GetPlayerBlock(), battle.GetEnergy(),
             Count(deck.GetDrawPile()), Count(deck.GetHand()), Count(deck.GetDiscardPile()));

    const std::vector<Enemy> &enemies = battle.GetEnemies();
    for (int i = 0; i < static_cast<int>(enemies.size()); i++)
    {
        const Enemy &enemy = enemies[i];
        TraceLog(LOG_INFO, "    Enemy %d %s | HP %d Block %d Strength %d | Intent %s %d",
                 i, GetEnemyDefinition(enemy.id).name, enemy.hp, enemy.block, enemy.strength,
                 GetIntentName(enemy.intent.type), enemy.intent.value);
    }
}

// Next living target
static int StepTarget(const Battle &battle, int target, int step)
{
    int count = static_cast<int>(battle.GetEnemies().size());
    for (int i = 1; i <= count; i++)
    {
        int index = ((target + step * i) % count + count) % count;
        if (battle.IsEnemyAlive(index))
        {
            return index;
        }
    }
    return target;
}

// Update
ScreenAction UpdateBattleScreen(BattleScreenState &state, Battle &battle)
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

    // Keep target on a living enemy
    if (!battle.IsEnemyAlive(state.target))
    {
        state.target = StepTarget(battle, state.target, 1);
    }

    // End turn
    if (IsKeyPressed(KEY_E))
    {
        LogBattle(battle, "Before EndTurn");
        battle.EndPlayerTurn();
        state.selectedCard = -1;
        LogBattle(battle, "After EndTurn");
        return ScreenAction::None;
    }

    // Change target
    if (IsKeyPressed(KEY_RIGHT))
    {
        state.target = StepTarget(battle, state.target, 1);
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        state.target = StepTarget(battle, state.target, -1);
    }

    // Select card
    const int cardKeys[Battle::handSize]{KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE};
    for (int i = 0; i < Battle::handSize; i++)
    {
        if (IsKeyPressed(cardKeys[i]) && battle.GetDeck().IsValidHandIndex(i))
        {
            state.selectedCard = i;
        }
    }

    // Play card
    if (IsKeyPressed(KEY_ENTER) && state.selectedCard != -1)
    {
        int card = state.selectedCard;
        bool played = battle.PlayCard(card, state.target);
        LogBattle(battle, TextFormat("PlayCard(%d, target %d) %s", card, state.target, played ? "accepted" : "rejected"));
        if (played)
        {
            state.selectedCard = -1;
        }
    }

    return ScreenAction::None;
}

// Player panel
static void DrawPlayerSide(const BattleScreenState &state, const Battle &battle)
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
        Color color = battle.CanPlayCard(i, state.target) ? DARKGREEN : GRAY;
        const char *marker = i == state.selectedCard ? ">" : " ";
        DrawTextLine(TextFormat("%s %d: %s | Cost %d | %s", marker, i + 1, card.name, card.cost, DescribeEffects(card).c_str()),
                     x, y, color);
    }
}

// Enemy panel
static void DrawEnemy(const BattleScreenState &state, const Battle &battle, int index, int x)
{
    const Enemy &enemy = battle.GetEnemies()[index];
    const EnemyDefinition &definition = GetEnemyDefinition(enemy.id);
    const int textX = x + padding;

    Color titleColor = battle.IsEnemyAlive(index) ? MAROON : GRAY;
    const char *title = index == state.target ? TextFormat("> %s", definition.name) : definition.name;
    int y = DrawSideTitle(x, title, titleColor);
    if (!battle.IsEnemyAlive(index))
    {
        DrawTextLine("Dead", textX, y, GRAY);
        return;
    }

    DrawTextLine(TextFormat("HP: %d / %d", enemy.hp, definition.maxHp), textX, y, BLACK);
    DrawTextLine(TextFormat("Block: %d", enemy.block), textX, y, BLACK);
    DrawTextLine(TextFormat("Strength: %d", enemy.strength), textX, y, BLACK);
    y += lineHeight;
    DrawTextLine(TextFormat("%s %d", GetIntentName(enemy.intent.type), enemy.intent.value), textX, y, MAROON);
}

static void DrawEnemySide(const BattleScreenState &state, const Battle &battle)
{
    int count = static_cast<int>(battle.GetEnemies().size());
    int columnWidth = panelWidth / count;
    for (int i = 0; i < count; i++)
    {
        DrawEnemy(state, battle, i, enemyX + i * columnWidth);
    }
}

// Draw
void DrawBattleScreen(const BattleScreenState &state, const Battle &battle)
{
    DrawCenteredText(TextFormat("BATTLE - Turn %d", battle.GetTurn()), 40, 40, RED);
    DrawPlayerSide(state, battle);
    DrawEnemySide(state, battle);

    const int y = panelTop + panelHeight + 50;
    switch (battle.GetResult())
    {
    case BattleResult::InProgress:
        DrawCenteredText("1-5: Select card | Left/Right: Target | Enter: Play | E: End turn", y, fontSize, GRAY);
        break;
    case BattleResult::Won:
        DrawCenteredText("WON! Press ENTER to continue", y, titleSize, DARKGREEN);
        break;
    case BattleResult::Lost:
        DrawCenteredText("LOST! Press ENTER to continue", y, titleSize, RED);
        break;
    }
}
