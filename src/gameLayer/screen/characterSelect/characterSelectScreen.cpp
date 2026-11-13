#include <raylib.h>
#include "characterSelectScreen.h"
#include "gameConfig.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetPanel()
{
    return {screenWidth / 2.0f - 360.0f, 220.0f, 720.0f, 560.0f};
}

static Rectangle GetConfirmButton()
{
    return {screenWidth / 2.0f - 168.0f, 840.0f, 336.0f, 96.0f};
}

ScreenAction UpdateCharacterSelectScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetConfirmButton()))
    {
        return ScreenAction::CharacterConfirmed;
    }

    return ScreenAction::None;
}

void DrawCharacterSelectScreen(const Assets &assets)
{
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(120);
    DrawTextOutlined(assets.font, "Choose your hero", {screenWidth / 2.0f, 130.0f}, titleText, goldColor);

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    int frame = static_cast<int>(GetTime() / 0.1) % idle.frameCount;
    DrawSprite(idle, frame, {screenWidth / 2.0f, panel.y + 330.0f}, false, WHITE);

    DrawTextOutlined(assets.font, "Knight", {screenWidth / 2.0f, panel.y + 400.0f}, titleText, goldColor);
    DrawTextCenter(assets.font, "80 HP - Strike, Defend, Heavy Strike", {screenWidth / 2.0f, panel.y + 470.0f}, smallText, peachColor);

    DrawButton(assets, GetConfirmButton(), "Confirm", true);
}
