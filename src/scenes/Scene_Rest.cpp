#include "scenes/Scene_Rest.h"
#include <algorithm>
#include "engine/GameConfig.h"
#include "engine/GameEngine.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

// The hero rests by a campfire in the middle of the screen, with the HP readout and the
// button below.
static constexpr Vector2 heroFeet{780.0f, 620.0f};
static constexpr Vector2 fireCenter{1180.0f, 540.0f};
static constexpr float fireScale = 12.0f;
static constexpr float healthY = 740.0f;
static constexpr float heartScale = 5.0f;

static Rectangle RestButton()
{
    return CenteredRect(840.0f, 400.0f, buttonHeight);
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
    if (action.isStart("REST") || isClicked(action, RestButton()))
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

    DrawBottomShade(heroFeet.y);
    DrawPortrait(m_heroIdle, m_heroIdle.frame(true), heroFeet);
    DrawIcon(assets.getTexture("RoomRest"), fireCenter, fireScale, WHITE);

    DrawIcon(assets.getTexture("Heart"), {screenWidth / 2.0f - 110.0f, healthY}, heartScale, WHITE);
    DrawTextOutlined(font, TextFormat("%d / %d", run.hp, run.maxHp), {screenWidth / 2.0f + 40.0f, healthY - bigTextNudge}, bigText, textColor);

    DrawButton(assets, RestButton(), TextFormat("Heal %d HP", restHeal), ButtonStyle::Gold, isHeld(RestButton()));
}
