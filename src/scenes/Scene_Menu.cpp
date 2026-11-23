#include "scenes/Scene_Menu.h"
#include <memory>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "scenes/Scene_CharacterSelect.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static Rectangle PlayButton()
{
    return CenteredRect(640.0f, 360.0f, buttonHeight);
}

static Rectangle ExitButton()
{
    return CenteredRect(750.0f, 360.0f, buttonHeight);
}

Scene_Menu::Scene_Menu(GameEngine *game) : Scene(game)
{
    registerAction(KEY_ENTER, "PLAY");
}

void Scene_Menu::update()
{
    sRender();
    m_currentFrame++;
}

void Scene_Menu::sDoAction(const Action &action)
{
    if (action.isStart("PLAY") || isClicked(action, PlayButton()))
    {
        m_game->changeScene("CHARACTER_SELECT", std::make_shared<Scene_CharacterSelect>(m_game), true);
    }
    else if (isClicked(action, ExitButton()))
    {
        m_game->quit();
    }
}

void Scene_Menu::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    DrawSceneBackdrop(assets, "Dungeon", 120);
    DrawHeading(assets.getFont("Title"), "KURIN", {screenWidth / 2.0f, 320.0f}, logoText, goldColor);
    DrawTextOutlined(font, "A dungeon deckbuilder", {screenWidth / 2.0f, 500.0f}, bigText, peachColor);
    DrawButton(assets, PlayButton(), "Play", ButtonStyle::Gold, isHeld(PlayButton()));
    DrawButton(assets, ExitButton(), "Exit", ButtonStyle::Plain, isHeld(ExitButton()));
}
