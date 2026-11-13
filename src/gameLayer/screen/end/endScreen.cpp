#include <raylib.h>
#include "endScreen.h"
#include "gameConfig.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetMenuButton()
{
    return {screenWidth / 2.0f - 216.0f, 640.0f, 432.0f, 96.0f};
}

ScreenAction UpdateEndScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetMenuButton()))
    {
        return ScreenAction::ReturnToMenu;
    }
    return ScreenAction::None;
}

void DrawEndScreen(bool won, const Assets &assets)
{
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(150);

    Rectangle ribbon{screenWidth / 2.0f - 420.0f, 330.0f, 840.0f, 17.0f * uiScale};
    DrawBox(won ? assets.victoryRibbon : assets.defeatRibbon, ribbon, WHITE);
    DrawTextOutlined(assets.font, won ? "Victory" : "Game Over", {screenWidth / 2.0f, ribbon.y + ribbon.height / 2.0f - 8.0f}, titleText, textColor);
    DrawTextCenter(assets.font, won ? "The Warden has fallen." : "Your run ends here.", {screenWidth / 2.0f, 540.0f}, bigText, peachColor);

    DrawButton(assets, GetMenuButton(), "Back to menu", true);
}
