#include <algorithm>
#include <string>
#include <vector>
#include <raylib.h>
#include "characterSelectScreen.h"
#include "gameConfig.h"
#include "cards/cardDatabase.h"
#include "characters/characterDatabase.h"
#include "relics/relicDatabase.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float panelWidth = 760.0f;
const float panelHeight = 560.0f;
const float panelGap = 60.0f;
const float panelY = 230.0f;

static Rectangle GetPanel(int index)
{
    float left = (screenWidth - panelWidth * characterCount - panelGap * (characterCount - 1)) / 2.0f;
    return {left + index * (panelWidth + panelGap), panelY, panelWidth, panelHeight};
}

static Rectangle GetConfirmButton()
{
    return CenteredRect(870.0f, 360.0f, buttonHeight);
}

static std::string DescribeDeck(const CharacterDefinition &character)
{
    std::vector<CardId> shown;
    std::string text;
    for (CardId id : character.startingDeck)
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

ScreenAction UpdateCharacterSelectScreen(CharacterSelectState &state)
{
    if (IsKeyPressed(KEY_LEFT))
    {
        state.selected = std::max(0, state.selected - 1);
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        state.selected = std::min(characterCount - 1, state.selected + 1);
    }
    for (int i = 0; i < characterCount; i++)
    {
        if (IsClicked(GetPanel(i)))
        {
            state.selected = i;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetConfirmButton()))
    {
        return ScreenAction::CharacterConfirmed;
    }
    return ScreenAction::None;
}

static void DrawCharacterPanel(const Assets &assets, int index, bool selected)
{
    CharacterId id = static_cast<CharacterId>(index);
    const CharacterDefinition &character = GetCharacter(id);
    Rectangle panel = GetPanel(index);
    float centerX = panel.x + panel.width / 2.0f;

    if (selected)
    {
        Rectangle outline{panel.x - uiScale, panel.y - uiScale, panel.width + uiScale * 2, panel.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawPanel(assets, panel);

    const SpriteSheet &idle = GetHeroSprites(assets, id).idle;
    float time = selected ? static_cast<float>(GetTime()) : 0.0f;
    DrawPortrait(idle, GetLoopFrame(idle, time), {centerX, panel.y + 300.0f});

    DrawHeading(assets.titleFont, character.name, {centerX, panel.y + 370.0f}, headingText, selected ? goldColor : textColor);
    DrawTextCenter(assets.font, TextFormat("%d HP", character.maxHp), {centerX, panel.y + 430.0f}, smallText, textColor);
    DrawTextCenter(assets.font, DescribeDeck(character).c_str(), {centerX, panel.y + 475.0f}, smallText, peachColor);
    const Relic &relic = GetRelic(character.starterRelic);
    DrawTextCenter(assets.font, TextFormat("%s: %s", relic.name, DescribeRelic(relic).c_str()), {centerX, panel.y + 520.0f}, smallText, tealColor);
}

void DrawCharacterSelectScreen(const CharacterSelectState &state, const Assets &assets)
{
    DrawSceneBackdrop(assets, assets.dungeon, 120, "Choose your hero");
    for (int i = 0; i < characterCount; i++)
    {
        DrawCharacterPanel(assets, i, i == state.selected);
    }
    DrawButton(assets, GetConfirmButton(), "Confirm", true);
}
