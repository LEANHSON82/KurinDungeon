#include "scenes/Scene_Menu.h"
#include <memory>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_CharacterSelect.h"
#include "ui/Text.h"
#include "ui/Widget.h"

Scene_Menu::Scene_Menu(GameEngine *game) : Scene(game)
{
    registerAction(KEY_ENTER, "PLAY");
}

Rectangle Scene_Menu::playButton() const
{
    return CenteredRect(600.0f, 336.0f, buttonHeight);
}

Rectangle Scene_Menu::exitButton() const
{
    return CenteredRect(720.0f, 336.0f, buttonHeight);
}

void Scene_Menu::update()
{
    sRender();
    m_currentFrame++;
}

void Scene_Menu::sDoAction(const Action &action)
{
    if ((action.name() == "PLAY" && action.type() == "START") || isClicked(action, playButton()))
    {
        m_game->changeScene("CHARACTER_SELECT", std::make_shared<Scene_CharacterSelect>(m_game), true);
    }
    else if (isClicked(action, exitButton()))
    {
        m_game->quit();
    }
}

void Scene_Menu::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    DrawSceneBackdrop(assets, "Dungeon", 120);
    DrawTextOutlined(font, "KURIN", {screenWidth / 2.0f, 340.0f}, logoText, goldColor);
    DrawTextOutlined(font, "A dungeon deckbuilder", {screenWidth / 2.0f, 460.0f}, bigText, peachColor);
    DrawPanel(assets, CenteredRect(560.0f, 480.0f, 300.0f));
    DrawButton(assets, playButton(), "Play", true, isHeld(playButton()));
    DrawButton(assets, exitButton(), "Exit", false, isHeld(exitButton()));
}
