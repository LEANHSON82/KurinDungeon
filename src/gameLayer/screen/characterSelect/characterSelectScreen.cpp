#include <algorithm>
#include <string>
#include <vector>
#include <raylib.h>
#include "characterSelectScreen.h"
#include "gameConfig.h"
#include "cards/cardDatabase.h"
#include "run/player.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

static Rectangle GetPanel()
{
    return CenteredRect(250.0f, 760.0f, 540.0f);
}

static Rectangle GetConfirmButton()
{
    return CenteredRect(850.0f, 360.0f, buttonHeight);
}

static std::string DescribeHero()
{
    std::vector<CardId> shown;
    std::string text = std::to_string(startingHp) + " HP - ";
    for (CardId id : GetStartingDeck())
    {
        if (std::find(shown.begin(), shown.end(), id) != shown.end())
        {
            continue;
        }
        if (!shown.empty())
        {
            text += ", ";
        }
        text += GetCard(id).name;
        shown.push_back(id);
    }
    return text;
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
    DrawSceneBackdrop(assets, assets.dungeon, 120, "Choose your hero");

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    DrawPortrait(idle, GetLoopFrame(idle, static_cast<float>(GetTime())), {screenWidth / 2.0f, panel.y + 320.0f});
    DrawHeading(assets.titleFont, "Knight", {screenWidth / 2.0f, panel.y + 400.0f}, headingText, goldColor);
    DrawTextCenter(assets.font, DescribeHero().c_str(), {screenWidth / 2.0f, panel.y + 470.0f}, smallText, peachColor);

    DrawButton(assets, GetConfirmButton(), "Confirm", true);
}
