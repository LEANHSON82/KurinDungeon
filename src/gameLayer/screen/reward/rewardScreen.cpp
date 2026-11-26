#include <raylib.h>
#include "rewardScreen.h"
#include "gameConfig.h"
#include "screen/cardView.h"
#include "screen/relicView.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float rowWidth = 860.0f;
const float rowGap = 40.0f;
const float firstRowY = 320.0f;
const float numberGap = 50.0f;
const float goldY = 240.0f;

static Rectangle GetChoiceRect(int index)
{
    return {(screenWidth - rowWidth) / 2.0f, firstRowY + index * (rowHeight + rowGap), rowWidth, rowHeight};
}

static Rectangle GetSkipButton()
{
    return CenteredRect(900.0f, 360.0f, buttonHeight);
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
    if (IsClicked(GetSkipButton()))
    {
        return ScreenAction::RewardSkipped;
    }

    return ScreenAction::None;
}

void DrawRewardScreen(const std::vector<CardId> &choices, int gold, const Assets &assets)
{
    DrawSceneBackdrop(assets, assets.dungeon, 150, "Choose a card", 150.0f);
    DrawGoldAmount(assets, TextFormat("+%d", gold), {screenWidth / 2.0f, goldY});

    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        Rectangle rect = GetChoiceRect(i);
        DrawCardRow(assets, choices[i], rect, IsHovered(rect));
        Vector2 number{rect.x - numberGap, rect.y + rect.height / 2.0f - bigTextNudge};
        DrawTextOutlined(assets.font, TextFormat("%d", i + 1), number, bigText, peachColor);
    }

    DrawButton(assets, GetSkipButton(), "Skip", false);
}
