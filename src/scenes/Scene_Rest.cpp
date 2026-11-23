#include "scenes/Scene_Rest.h"
#include <algorithm>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static Rectangle Panel()
{
    return CenteredRect(220.0f, 720.0f, 520.0f);
}

static Rectangle RestButton()
{
    return CenteredRect(800.0f, 384.0f, buttonHeight);
}

Scene_Rest::Scene_Rest(GameEngine *game) : Scene(game), m_heroIdle(game->assets().getAnimation("KnightIdle"))
{
    registerAction(KEY_ENTER, "REST");
}

void Scene_Rest::rest()
{
    Run &run = m_game->currentRun();
    run.hp = std::min(run.hp + restHeal, run.maxHp);
    run.roomFinished = true;
    m_game->changeScene("MAP", nullptr, true);
}

void Scene_Rest::update()
{
    m_heroIdle.update();
    sRender();
    m_currentFrame++;
}

void Scene_Rest::sDoAction(const Action &action)
{
    if ((action.name() == "REST" && action.type() == "START") || isClicked(action, RestButton()))
    {
        rest();
    }
}

void Scene_Rest::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    const Run &run = m_game->currentRun();
    DrawSceneBackdrop(assets, "Graveyard", 120, "Rest");

    Rectangle panel = Panel();
    DrawPanel(assets, panel);
    DrawSprite(m_heroIdle, m_heroIdle.frame(true), {screenWidth / 2.0f - 110.0f, panel.y + 320.0f}, false, WHITE);
    DrawIcon(assets.getTexture("RoomRest"), {screenWidth / 2.0f + 110.0f, panel.y + 270.0f}, uiScale, WHITE);

    DrawIcon(assets.getTexture("Heart"), {screenWidth / 2.0f - 90.0f, panel.y + 430.0f}, uiScale, WHITE);
    DrawTextLeft(font, TextFormat("%d / %d", run.hp, run.maxHp), {screenWidth / 2.0f - 50.0f, panel.y + 398.0f}, bigText, textColor);

    DrawButton(assets, RestButton(), TextFormat("Heal %d HP", restHeal), true, isHeld(RestButton()));
}
