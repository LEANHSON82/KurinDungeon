#include <raylib.h>
#include "core/state/run.h"
#include "game/assets/assets.h"
#include "game/gameConfig.h"
#include "game/scenes/scenes.h"
#include "game/ui/cardView.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static const float rowWidth = 760.0f;
static const float rowGap = 40.0f;
static const float firstRowY = 320.0f;
static const float numberGap = 50.0f;

static Rectangle GetChoiceRect(int index)
{
    return {(screenWidth - rowWidth) / 2.0f, firstRowY + index * (rowHeight + rowGap), rowWidth, rowHeight};
}

void UpdateRewardScene(ecs::Registry &registry)
{
    int count = static_cast<int>(registry.Resource<RunState>().rewardChoices.size());
    for (int i = 0; i < count; i++)
    {
        if (IsKeyPressed(KEY_ONE + i) || IsClicked(GetChoiceRect(i)))
        {
            PushRunCommand(registry, {RunCommandType::TakeReward, ecs::nullEntity, i, 0});
            return;
        }
    }
}

void DrawRewardScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    const std::vector<CardId> &choices = registry.Resource<RunState>().rewardChoices;

    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(150);
    DrawTextOutlined(assets.font, "Choose a card", {screenWidth / 2.0f, 200.0f}, titleText, goldColor);

    for (int i = 0; i < static_cast<int>(choices.size()); i++)
    {
        Rectangle rect = GetChoiceRect(i);
        DrawCardRow(assets, choices[i], rect, IsHovered(rect));
        DrawTextOutlined(assets.font, TextFormat("%d", i + 1), {rect.x - numberGap, rect.y + rect.height / 2.0f - bigTextNudge}, bigText, peachColor);
    }
}
