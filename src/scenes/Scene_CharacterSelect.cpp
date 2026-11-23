#include "scenes/Scene_CharacterSelect.h"
#include <algorithm>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/map/Scene_Map.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static Rectangle Panel()
{
    return CenteredRect(220.0f, 720.0f, 560.0f);
}

static Rectangle ConfirmButton()
{
    return CenteredRect(840.0f, 336.0f, buttonHeight);
}

// "80 HP - Strike, Defend, Heavy Strike", built from the starting deck.
static std::string DescribeHero()
{
    std::vector<CardId> shown;
    std::string text = std::to_string(startingHp) + " HP - ";
    for (CardId id : GetStartingDeck())
    {
        if (std::find(shown.begin(), shown.end(), id) != shown.end())
        {
            continue;
        }
        if (!shown.empty())
        {
            text += ", ";
        }
        text += GetCard(id).name;
        shown.push_back(id);
    }
    return text;
}

Scene_CharacterSelect::Scene_CharacterSelect(GameEngine *game)
    : Scene(game), m_heroIdle(game->assets().getAnimation("KnightIdle"))
{
    registerAction(KEY_ENTER, "CONFIRM");
}

void Scene_CharacterSelect::confirm()
{
    m_game->startRun(std::random_device{}());
    m_game->changeScene("MAP", std::make_shared<Scene_Map>(m_game), true);
}

void Scene_CharacterSelect::update()
{
    m_heroIdle.update();
    sRender();
    m_currentFrame++;
}

void Scene_CharacterSelect::sDoAction(const Action &action)
{
    if (action.isStart("CONFIRM") || isClicked(action, ConfirmButton()))
    {
        confirm();
    }
}

void Scene_CharacterSelect::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    DrawSceneBackdrop(assets, "Dungeon", 120, "Choose your hero");

    Rectangle panel = Panel();
    DrawPanel(assets, panel);
    DrawSprite(m_heroIdle, m_heroIdle.frame(true), {screenWidth / 2.0f, panel.y + 330.0f}, false, WHITE);
    DrawTextOutlined(font, "Knight", {screenWidth / 2.0f, panel.y + 400.0f}, titleText, goldColor);
    DrawTextCenter(font, DescribeHero().c_str(), {screenWidth / 2.0f, panel.y + 470.0f}, smallText, peachColor);

    DrawButton(assets, ConfirmButton(), "Confirm", ButtonStyle::Gold, isHeld(ConfirmButton()));
}
