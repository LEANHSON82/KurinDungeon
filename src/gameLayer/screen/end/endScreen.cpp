#include <string>
#include <raylib.h>
#include "endScreen.h"
#include "gameConfig.h"
#include "combat/enemies/enemyDatabase.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetMenuButton()
{
    return CenteredRect(640.0f, 432.0f, buttonHeight);
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
    DrawSceneBackdrop(assets, assets.dungeon, 150);

    Rectangle ribbon = CenteredRect(310.0f, 900.0f, 120.0f);
    DrawBox(won ? assets.victoryRibbon : assets.defeatRibbon, ribbon, WHITE);
    Vector2 titleCenter{screenWidth / 2.0f, ribbon.y + ribbon.height / 2.0f - titleTextNudge};
    DrawHeading(assets.titleFont, won ? "Victory" : "Game Over", titleCenter, headingText, textColor);
    std::string message = won ? std::string("The ") + GetEnemyDefinition(bossEnemy).name + " has fallen." : "Your run ends here.";
    DrawTextCenter(assets.font, message.c_str(), {screenWidth / 2.0f, 540.0f}, bigText, peachColor);

    DrawButton(assets, GetMenuButton(), "Back to menu", true);
}
