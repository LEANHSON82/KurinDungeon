#include <raylib.h>
#include "restScreen.h"
#include "gameConfig.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const Vector2 heroFeet{780.0f, 620.0f};
const Vector2 fireCenter{1180.0f, 540.0f};
const float fireScale = 12.0f;
const float healthY = 740.0f;
const float heartScale = 5.0f;

static Rectangle GetRestButton()
{
    return CenteredRect(840.0f, 400.0f, buttonHeight);
}

ScreenAction UpdateRestScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetRestButton()))
    {
        return ScreenAction::RestTaken;
    }
    return ScreenAction::None;
}

void DrawRestScreen(const PlayerState &player, const Assets &assets)
{
    DrawSceneBackdrop(assets, assets.graveyard, 120, "Rest");

    DrawBottomShade(heroFeet.y);
    const SpriteSheet &idle = GetHeroSprites(assets, player.character).idle;
    DrawPortrait(idle, GetLoopFrame(idle, static_cast<float>(GetTime())), heroFeet);
    DrawIcon(assets.restRoom, fireCenter, fireScale, WHITE);

    DrawIcon(assets.heartIcon, {screenWidth / 2.0f - 110.0f, healthY}, heartScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d / %d", player.hp, player.maxHp), {screenWidth / 2.0f + 40.0f, healthY - bigTextNudge}, bigText, textColor);

    DrawButton(assets, GetRestButton(), TextFormat("Heal %d HP", restHeal), true);
}
