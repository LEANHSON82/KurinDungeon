#include <raylib.h>
#include "restScreen.h"
#include "gameConfig.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetPanel()
{
    return {screenWidth / 2.0f - 360.0f, 220.0f, 720.0f, 520.0f};
}

static Rectangle GetRestButton()
{
    return {screenWidth / 2.0f - 192.0f, 800.0f, 384.0f, 96.0f};
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
    DrawBackground(assets.mapBackground, WHITE);
    DrawScreenShade(120);
    DrawTextOutlined(assets.font, "Rest", {screenWidth / 2.0f, 130.0f}, titleText, goldColor);

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    int frame = static_cast<int>(GetTime() / 0.1) % idle.frameCount;
    DrawSprite(idle, frame, {screenWidth / 2.0f - 110.0f, panel.y + 320.0f}, false, WHITE);
    DrawIcon(assets.restRoom, {screenWidth / 2.0f + 110.0f, panel.y + 270.0f}, uiScale, WHITE);

    DrawIcon(assets.heartIcon, {screenWidth / 2.0f - 90.0f, panel.y + 430.0f}, uiScale, WHITE);
    DrawTextLeft(assets.font, TextFormat("%d / %d", player.hp, player.maxHp), {screenWidth / 2.0f - 50.0f, panel.y + 398.0f}, bigText, textColor);

    DrawButton(assets, GetRestButton(), TextFormat("Heal %d HP", restHeal), true);
}
