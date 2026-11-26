#include <raylib.h>
#include "menuScreen.h"
#include "gameConfig.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float firstButtonY = 640.0f;
const float buttonGap = 110.0f;
const float menuButtonWidth = 360.0f;

static Rectangle GetMenuButton(int row)
{
    return CenteredRect(firstButtonY + row * buttonGap, menuButtonWidth, buttonHeight);
}

static Rectangle GetContinueButton()
{
    return GetMenuButton(0);
}

static Rectangle GetPlayButton(bool hasSave)
{
    return GetMenuButton(hasSave ? 1 : 0);
}

static Rectangle GetExitButton(bool hasSave)
{
    return GetMenuButton(hasSave ? 2 : 1);
}

ScreenAction UpdateMenuScreen(bool hasSave)
{
    if (hasSave && (IsKeyPressed(KEY_ENTER) || IsClicked(GetContinueButton())))
    {
        return ScreenAction::ContinuePressed;
    }
    if ((!hasSave && IsKeyPressed(KEY_ENTER)) || IsClicked(GetPlayButton(hasSave)))
    {
        return ScreenAction::StartPressed;
    }
    if (IsClicked(GetExitButton(hasSave)))
    {
        return ScreenAction::QuitPressed;
    }
    return ScreenAction::None;
}

void DrawMenuScreen(bool hasSave, const Assets &assets)
{
    DrawSceneBackdrop(assets, assets.dungeon, 120);
    DrawHeading(assets.titleFont, "KURIN", {screenWidth / 2.0f, 320.0f}, logoText, goldColor);
    DrawTextOutlined(assets.font, "A dungeon deckbuilder", {screenWidth / 2.0f, 500.0f}, bigText, peachColor);
    if (hasSave)
    {
        DrawButton(assets, GetContinueButton(), "Continue", true);
    }
    DrawButton(assets, GetPlayButton(hasSave), hasSave ? "New game" : "Play", !hasSave);
    DrawButton(assets, GetExitButton(hasSave), "Exit", false);
}
