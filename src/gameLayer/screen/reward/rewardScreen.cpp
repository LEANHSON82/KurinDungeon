#include <raylib.h>
#include "rewardScreen.h"
#include "cards/cardDatabase.h"

// Update
ScreenAction UpdateRewardScreen(const std::vector<CardId> &choices, int &chosenIndex)
{
    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
        {
            chosenIndex = i;
            return ScreenAction::CardChosen;
        }
    }

    return ScreenAction::None;
}

// Draw
void DrawRewardScreen(const std::vector<CardId> &choices)
{
    DrawText("CHOOSE A CARD", 20, 20, 40, RED);

    int y = 100;
    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        const Card &card = GetCard(choices[i]);
        DrawText(TextFormat("[%d] %s (cost %d) - %s", i + 1, card.name, card.cost,
                            DescribeEffects(card).c_str()),
                 20, y, 30, BLACK);
        y += 45;
    }
}
