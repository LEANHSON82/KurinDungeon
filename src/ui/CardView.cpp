#include "ui/CardView.h"
#include <algorithm>
#include <string>
#include "ui/Image.h"
#include "ui/Style.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static constexpr float tooltipMinWidth = 420.0f;
static constexpr float tooltipPadding = 60.0f;
static constexpr float tooltipHeight = 192.0f;
static constexpr float tooltipNameY = 44.0f; // text lines, measured from the top of the tooltip
static constexpr float tooltipKindY = 100.0f;
static constexpr float tooltipEffectsY = 144.0f;
static constexpr float costInset = 12.0f; // the cost number sits in the top left corner of a slot
static constexpr float rowPadding = 30.0f;
static constexpr float rowTextGap = 36.0f; // between the card picture and its text
static constexpr float rowNameY = 24.0f;
static constexpr float rowDetailsY = 88.0f;

// A gold border, one art pixel wide, around a selected or highlighted card.
static void DrawGoldOutline(Rectangle rect)
{
    Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
    DrawRectangleRec(outline, goldColor);
}

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected)
{
    const Card &card = GetCard(id);
    Color tint = playable ? WHITE : unplayableTint;
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f};

    if (selected)
    {
        DrawGoldOutline(rect);
    }
    DrawIcon(assets.getTexture(card.art), center, slotScale, tint);
    DrawNumber(assets, card.cost, {rect.x + costInset, rect.y + costInset}, detailScale, playable ? WHITE : GRAY);
}

void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter, const std::function<int(int)> &damage)
{
    const Card &card = GetCard(id);
    const Font &font = assets.getFont("Main");
    std::string effects = DescribeEffects(card, damage);
    float textWidth = MeasureTextEx(font, effects.c_str(), smallText, 0.0f).x;
    float width = std::max(tooltipMinWidth, textWidth + tooltipPadding);
    Rectangle box{bottomCenter.x - width / 2.0f, bottomCenter.y - tooltipHeight, width, tooltipHeight};
    DrawPanel(assets, box);

    float x = box.x + box.width / 2.0f;
    DrawTextOutlined(font, card.name, {x, box.y + tooltipNameY}, bigText, goldColor);
    DrawTextCenter(font, TextFormat("%s - Cost %d", GetKindName(card.kind), card.cost), {x, box.y + tooltipKindY}, smallText, peachColor);
    DrawTextCenter(font, effects.c_str(), {x, box.y + tooltipEffectsY}, smallText, textColor);
}

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted)
{
    const Card &card = GetCard(id);
    const Font &font = assets.getFont("Main");
    if (highlighted)
    {
        DrawGoldOutline(rect);
    }
    DrawPanel(assets, rect);

    Rectangle slot{rect.x + rowPadding, rect.y + (rect.height - slotSize) / 2.0f, slotSize, slotSize};
    DrawCardSlot(assets, id, slot, true, false);

    float textX = slot.x + slot.width + rowTextGap;
    std::string details = std::string(GetKindName(card.kind)) + " - " + DescribeEffects(card);
    DrawTextLeft(font, card.name, {textX, rect.y + rowNameY}, bigText, highlighted ? goldColor : textColor);
    DrawTextLeft(font, details.c_str(), {textX, rect.y + rowDetailsY}, smallText, peachColor);
}
