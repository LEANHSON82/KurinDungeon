#include "scenes/Scene_End.h"
#include <memory>
#include <string>
#include "data/Enemies.h"
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_Menu.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static Rectangle MenuButton()
{
    return CenteredRect(640.0f, 432.0f, buttonHeight);
}

Scene_End::Scene_End(GameEngine *game, bool won) : Scene(game), m_won(won)
{
    registerAction(KEY_ENTER, "MENU");
}

void Scene_End::update()
{
    sRender();
    m_currentFrame++;
}

void Scene_End::sDoAction(const Action &action)
{
    if (action.isStart("MENU") || isClicked(action, MenuButton()))
    {
        m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game), true);
    }
}

void Scene_End::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    DrawSceneBackdrop(assets, "Dungeon", 150);

    Rectangle ribbon = CenteredRect(330.0f, 840.0f, 17.0f * uiScale);
    DrawBox(assets.getBox(m_won ? "VictoryRibbon" : "DefeatRibbon"), ribbon, WHITE);
    Vector2 titleCenter{screenWidth / 2.0f, ribbon.y + ribbon.height / 2.0f - titleTextNudge};
    DrawTextOutlined(font, m_won ? "Victory" : "Game Over", titleCenter, titleText, textColor);
    std::string message = m_won ? std::string("The ") + GetEnemyDefinition(bossEnemy).name + " has fallen." : "Your run ends here.";
    DrawTextCenter(font, message.c_str(), {screenWidth / 2.0f, 540.0f}, bigText, peachColor);

    DrawButton(assets, MenuButton(), "Back to menu", ButtonStyle::Gold, isHeld(MenuButton()));
}
