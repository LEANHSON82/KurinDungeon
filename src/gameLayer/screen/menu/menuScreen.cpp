#include <raylib.h>
#include "menuScreen.h"
#include "gameConfig.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetPanel()
{
    return {screenWidth / 2.0f - 240.0f, 560.0f, 480.0f, 300.0f};
}

static Rectangle GetPlayButton()
{
    return {screenWidth / 2.0f - 168.0f, 600.0f, 336.0f, 96.0f};
}

static Rectangle GetExitButton()
{
    return {screenWidth / 2.0f - 168.0f, 720.0f, 336.0f, 96.0f};
}

ScreenAction UpdateMenuScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetPlayButton()))
    {
        return ScreenAction::StartPressed;
    }
    if (IsClicked(GetExitButton()))
    {
        return ScreenAction::QuitPressed;
    }
    return ScreenAction::None;
}

void DrawMenuScreen(const Assets &assets)
{
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(120);
    DrawTextOutlined(assets.font, "KURIN", {screenWidth / 2.0f, 340.0f}, 240.0f, goldColor);
    DrawTextOutlined(assets.font, "A dungeon deckbuilder", {screenWidth / 2.0f, 460.0f}, bigText, peachColor);
    DrawPanel(assets, GetPanel());
    DrawButton(assets, GetPlayButton(), "Play", true);
    DrawButton(assets, GetExitButton(), "Exit", false);
}
