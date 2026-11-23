#include "scenes/Scene_Reward.h"
#include <string>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "ui/CardView.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static constexpr float rowWidth = 860.0f;
static constexpr float rowGap = 40.0f;
static constexpr float firstRowY = 320.0f;
static constexpr float numberGap = 50.0f;

static Rectangle ChoiceRect(int index)
{
    return {(screenWidth - rowWidth) / 2.0f, firstRowY + index * (rowHeight + rowGap), rowWidth, rowHeight};
}

Scene_Reward::Scene_Reward(GameEngine *game) : Scene(game)
{
    m_choices = RollRewards(game->currentRun().rng, rewardChoiceCount);
    for (int i = 0; i < static_cast<int>(m_choices.size()); i++)
    {
        registerAction(KEY_ONE + i, "CHOOSE_" + std::to_string(i + 1));
    }
}

void Scene_Reward::choose(int index)
{
    Run &run = m_game->currentRun();
    run.deck.push_back(m_choices[index]);
    run.roomFinished = true;
    m_game->changeScene("MAP", nullptr, true);
}

void Scene_Reward::update()
{
    sRender();
    m_currentFrame++;
}

void Scene_Reward::sDoAction(const Action &action)
{
    for (int i = 0; i < static_cast<int>(m_choices.size()); i++)
    {
        bool key = action.name() == "CHOOSE_" + std::to_string(i + 1) && action.type() == "START";
        if (key || isClicked(action, ChoiceRect(i)))
        {
            choose(i);
            return;
        }
    }
}

void Scene_Reward::sRender()
{
    const Assets &assets = m_game->assets();
    DrawSceneBackdrop(assets, "Dungeon", 150, "Choose a card", 200.0f);

    for (int i = 0; i < static_cast<int>(m_choices.size()); i++)
    {
        Rectangle rect = ChoiceRect(i);
        DrawCardRow(assets, m_choices[i], rect, IsHovered(rect));
        Vector2 number{rect.x - numberGap, rect.y + rect.height / 2.0f - bigTextNudge};
        DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", i + 1), number, bigText, peachColor);
    }
}
