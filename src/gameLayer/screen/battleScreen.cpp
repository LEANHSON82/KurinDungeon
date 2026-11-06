#include <raylib.h>
#include "battleScreen.h"
#include "cards/cardDatabase.h"

ScreenAction UpdateBattleScreen()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        return ScreenAction::returnToMap;
    }

    return ScreenAction::none;
}

void DrawBattleScreen(const PlayerState &player)
{
    DrawText("BATTLE", 100, 50, 40, RED);

    DrawText(TextFormat("HP: %d / %d", player.hp, player.maxHp),
             100, 110, 25, BLACK);
    DrawText(TextFormat("Gold: %d", player.gold),
             100, 150, 25, BLACK);

    int cardCount = static_cast<int>(player.masterDeck.size());
    DrawText(TextFormat("Master Deck: %d cards", cardCount),
             100, 190, 25, BLACK);

    const CardId cardIds[]{
        CardId::Strike,
        CardId::Defend,
        CardId::HeavyStrike,
    };

    for (int i = 0; i < 3; i++)
    {
        const Card &card = GetCard(cardIds[i]);
        DrawText(TextFormat("%s | Energy: %d | Damage: %d | Block: %d",
                            card.name, card.cost, card.damage, card.block),
                 100, 270 + i * 50, 25, BLACK);
    }

    DrawText("ENTER: Return to Map", 100, 470, 25, GRAY);
}
