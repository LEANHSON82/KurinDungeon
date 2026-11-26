#include <raylib.h>
#include "rewardScreen.h"
#include "gameConfig.h"
#include "screen/cardView.h"
#include "screen/relicView.h"
#include "relics/relicDatabase.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float rowWidth = 860.0f;
const float rowGap = 40.0f;
const float firstRowY = 320.0f;
const float numberGap = 50.0f;
const float goldY = 240.0f;
const float lootGap = 220.0f;
const float relicIconScale = 4.0f;

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

static void DrawLoot(int gold, bool hasRelic, RelicId relic, const Assets &assets)
{
    if (!hasRelic)
    {
        DrawGoldAmount(assets, TextFormat("+%d", gold), {screenWidth / 2.0f, goldY});
        return;
    }

    DrawGoldAmount(assets, TextFormat("+%d", gold), {screenWidth / 2.0f - lootGap, goldY});
    float relicX = screenWidth / 2.0f + 40.0f;
    DrawIcon(assets.relicArt[static_cast<int>(relic)], {relicX, goldY}, relicIconScale, WHITE);
    DrawTextLeft(assets.font, GetRelic(relic).name, {relicX + 50.0f, goldY - 30.0f}, bigText, goldColor);
}

void DrawRewardScreen(const std::vector<CardId> &choices, int gold, bool hasRelic, RelicId relic, const Assets &assets)
{
    DrawSceneBackdrop(assets, assets.dungeon, 150, "Choose a card", 150.0f);
    DrawLoot(gold, hasRelic, relic, assets);

    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        Rectangle rect = GetChoiceRect(i);
        DrawCardRow(assets, choices[i], rect, IsHovered(rect));
        Vector2 number{rect.x - numberGap, rect.y + rect.height / 2.0f - bigTextNudge};
        DrawTextOutlined(assets.font, TextFormat("%d", i + 1), number, bigText, peachColor);
    }

    DrawButton(assets, GetSkipButton(), "Skip", false);
}
