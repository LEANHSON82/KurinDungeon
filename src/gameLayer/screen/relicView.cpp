#include "relicView.h"
#include <algorithm>
#include <string>
#include "gameConfig.h"
#include "relics/relicDatabase.h"
#include "ui/image.h"
#include "ui/style.h"
#include "ui/text.h"
#include "ui/widget.h"

const float relicIconScale = 4.0f;
const float relicSlot = 80.0f;
const int relicsPerRow = 4;
const float barPadding = 20.0f;
const float tooltipMinWidth = 380.0f;
const float tooltipPadding = 60.0f;
const float tooltipHeight = 140.0f;
const float tooltipGap = 12.0f;
const float rowIconScale = 6.0f;
const float rowPadding = 30.0f;
const float rowTextGap = 36.0f;
const float rowNameY = 24.0f;
const float rowDetailsY = 88.0f;

static Vector2 GetRelicCenter(Vector2 topLeft, int index)
{
    int row = index / relicsPerRow;
    int column = index % relicsPerRow;
    return {topLeft.x + barPadding + relicSlot * (column + 0.5f), topLeft.y + barPadding + relicSlot * (row + 0.5f)};
}

static void DrawRelicTooltip(const Assets &assets, RelicId id, Vector2 topLeft)
{
    const Relic &relic = GetRelic(id);
    std::string description = DescribeRelic(relic);
    float textWidth = MeasureTextEx(assets.font, description.c_str(), smallText, 0.0f).x;
    float width = std::max(tooltipMinWidth, textWidth + tooltipPadding);
    float x = std::min(topLeft.x, screenWidth - width - tooltipGap);
    Rectangle box{x, topLeft.y, width, tooltipHeight};
    DrawPanel(assets, box);

    float centerX = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, relic.name, {centerX, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(assets.font, description.c_str(), {centerX, box.y + 100.0f}, smallText, textColor);
}

void DrawRelicBar(const Assets &assets, const std::vector<RelicId> &relics, Vector2 topLeft)
{
    int count = static_cast<int>(relics.size());
    if (count == 0)
    {
        return;
    }

    int rows = (count + relicsPerRow - 1) / relicsPerRow;
    int columns = std::min(count, relicsPerRow);
    Rectangle panel{topLeft.x, topLeft.y, barPadding * 2 + relicSlot * columns, barPadding * 2 + relicSlot * rows};
    DrawPanel(assets, panel);

    int hovered = -1;
    for (int i = 0; i < count; i++)
    {
        Vector2 center = GetRelicCenter(topLeft, i);
        DrawIcon(assets.relicArt[static_cast<int>(relics[i])], center, relicIconScale, WHITE);
        Rectangle slot{center.x - relicSlot / 2.0f, center.y - relicSlot / 2.0f, relicSlot, relicSlot};
        if (IsHovered(slot))
        {
            hovered = i;
        }
    }

    if (hovered != -1)
    {
        DrawRelicTooltip(assets, relics[hovered], {panel.x, panel.y + panel.height + tooltipGap});
    }
}

void DrawRelicRow(const Assets &assets, RelicId id, Rectangle rect, bool highlighted)
{
    const Relic &relic = GetRelic(id);
    if (highlighted)
    {
        Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawPanel(assets, rect);

    float iconSize = 16.0f * rowIconScale;
    Vector2 iconCenter{rect.x + rowPadding + iconSize / 2.0f, rect.y + rect.height / 2.0f};
    DrawIcon(assets.relicArt[static_cast<int>(id)], iconCenter, rowIconScale, WHITE);

    float textX = rect.x + rowPadding + iconSize + rowTextGap;
    DrawTextLeft(assets.font, TextFormat("Relic - %s", relic.name), {textX, rect.y + rowNameY}, bigText, highlighted ? goldColor : textColor);
    DrawTextLeft(assets.font, DescribeRelic(relic).c_str(), {textX, rect.y + rowDetailsY}, smallText, peachColor);
}

void DrawGoldAmount(const Assets &assets, const char *text, Vector2 center)
{
    const float iconWidth = 16.0f * goldIconScale;
    const float spacing = 14.0f;
    float textWidth = MeasureTextEx(assets.font, text, bigText, 0.0f).x;
    float left = center.x - (iconWidth + spacing + textWidth) / 2.0f;
    DrawIcon(assets.goldIcon, {left + iconWidth / 2.0f, center.y}, goldIconScale, WHITE);
    DrawTextOutlined(assets.font, text, {left + iconWidth + spacing + textWidth / 2.0f, center.y - bigTextNudge}, bigText, goldColor);
}
