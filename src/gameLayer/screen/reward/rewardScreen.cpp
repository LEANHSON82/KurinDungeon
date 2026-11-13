#include <raylib.h>
#include "rewardScreen.h"
#include "gameConfig.h"
#include "screen/cardView.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static const float rowWidth = 760.0f;
static const float rowGap = 40.0f;
static const float firstRowY = 320.0f;

static Rectangle GetChoiceRect(int index)
{
    return {(screenWidth - rowWidth) / 2.0f, firstRowY + index * (rowHeight + rowGap), rowWidth, rowHeight};
}

ScreenAction UpdateRewardScreen(const std::vector<CardId> &choices, int &chosenIndex)
{
    int count = static_cast<int>(choices.size());
    for (int i = 0; i < count; i++)
    {
        if (IsKeyPressed(KEY_ONE + i) || IsClicked(GetChoiceRect(i)))
        {
            chosenIndex = i;
            return ScreenAction::CardChosen;
        }
    }

    return ScreenAction::None;
}

void DrawRewardScreen(const std::vector<CardId> &choices, const Assets &assets)
{
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(150);
    DrawTextOutlined(assets.font, "Choose a card", {screenWidth / 2.0f, 200.0f}, titleText, goldColor);

    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        Rectangle rect = GetChoiceRect(i);
        DrawCardRow(assets, choices[i], rect, IsHovered(rect));
        DrawTextOutlined(assets.font, TextFormat("%d", i + 1), {rect.x - 50.0f, rect.y + rect.height / 2.0f - 6.0f}, bigText, peachColor);
    }
}
