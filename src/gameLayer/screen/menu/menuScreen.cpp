#include <raylib.h>
#include "menuScreen.h"
#include "gameConfig.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetPlayButton()
{
    return CenteredRect(640.0f, 360.0f, buttonHeight);
}

static Rectangle GetExitButton()
{
    return CenteredRect(750.0f, 360.0f, buttonHeight);
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
    DrawSceneBackdrop(assets, assets.dungeon, 120);
    DrawHeading(assets.titleFont, "KURIN", {screenWidth / 2.0f, 320.0f}, logoText, goldColor);
    DrawTextOutlined(assets.font, "A dungeon deckbuilder", {screenWidth / 2.0f, 500.0f}, bigText, peachColor);
    DrawButton(assets, GetPlayButton(), "Play", true);
    DrawButton(assets, GetExitButton(), "Exit", false);
}
