#include <raylib.h>
#include "treasureScreen.h"
#include "gameConfig.h"
#include "run/runState.h"
#include "screen/relicView.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float chestY = 380.0f;
const float chestScale = 14.0f;
const float rowWidth = 860.0f;
const float rowY = 560.0f;
const float rowHeight = 150.0f;

static Rectangle GetTakeButton()
{
    return CenteredRect(840.0f, 400.0f, buttonHeight);
}

ScreenAction UpdateTreasureScreen()
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetTakeButton()))
    {
        return ScreenAction::TreasureTaken;
    }
    return ScreenAction::None;
}

void DrawTreasureScreen(bool hasRelic, RelicId relic, const Assets &assets)
{
    DrawPlainBackdrop(assets, "Treasure");
    DrawIcon(assets.treasureRoom, {screenWidth / 2.0f, chestY}, chestScale, WHITE);

    if (hasRelic)
    {
        DrawRelicRow(assets, relic, CenteredRect(rowY, rowWidth, rowHeight), false);
        DrawButton(assets, GetTakeButton(), "Take relic", true);
        return;
    }
    DrawGoldAmount(assets, TextFormat("+%d", noRelicGold), {screenWidth / 2.0f, rowY + rowHeight / 2.0f});
    DrawButton(assets, GetTakeButton(), "Take gold", true);
}
