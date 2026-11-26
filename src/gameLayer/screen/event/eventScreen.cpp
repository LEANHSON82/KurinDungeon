#include <raylib.h>
#include "eventScreen.h"
#include "gameConfig.h"
#include "events/eventDatabase.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float choiceWidth = 1000.0f;
const float firstChoiceY = 640.0f;
const float choiceGap = 110.0f;
const float iconY = 360.0f;
const float textY = 520.0f;
const float eventIconScale = 10.0f;

static Rectangle GetChoiceButton(int index)
{
    return CenteredRect(firstChoiceY + index * choiceGap, choiceWidth, buttonHeight);
}

ScreenAction UpdateEventScreen(EventId id, int &chosenIndex)
{
    const Event &event = GetEvent(id);
    for (int i = 0; i < static_cast<int>(event.choices.size()); i++)
    {
        if (IsKeyPressed(KEY_ONE + i) || IsClicked(GetChoiceButton(i)))
        {
            chosenIndex = i;
            return ScreenAction::EventChoiceMade;
        }
    }
    return ScreenAction::None;
}

void DrawEventScreen(EventId id, const Assets &assets)
{
    const Event &event = GetEvent(id);
    DrawPlainBackdrop(assets, event.name);
    DrawIcon(assets.eventRoom, {screenWidth / 2.0f, iconY}, eventIconScale, WHITE);
    DrawTextCenter(assets.font, event.text, {screenWidth / 2.0f, textY}, bigText, peachColor);

    for (int i = 0; i < static_cast<int>(event.choices.size()); i++)
    {
        DrawButton(assets, GetChoiceButton(i), TextFormat("%d. %s", i + 1, event.choices[i].label), i == 0);
    }
}
