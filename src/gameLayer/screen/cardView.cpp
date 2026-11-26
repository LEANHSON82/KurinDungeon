#include "cardView.h"
#include <algorithm>
#include <string>
#include "cards/cardDatabase.h"
#include "ui/image.h"
#include "ui/style.h"
#include "ui/text.h"
#include "ui/widget.h"

const float tooltipMinWidth = 420.0f;
const float tooltipPadding = 60.0f;
const float tooltipHeight = 192.0f;
const float tooltipNameY = 44.0f;
const float tooltipKindY = 100.0f;
const float tooltipEffectsY = 144.0f;
const float costInset = 12.0f;
const float rowPadding = 30.0f;
const float rowTextGap = 36.0f;
const float rowNameY = 24.0f;
const float rowDetailsY = 88.0f;

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
    DrawIcon(assets.cardArt[static_cast<int>(id)], center, slotScale, tint);
    DrawNumber(assets, card.cost, {rect.x + costInset, rect.y + costInset}, detailScale, playable ? WHITE : GRAY);
}

void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter, const Statuses &source, const Statuses &victim)
{
    const Card &card = GetCard(id);
    std::string effects = DescribeEffects(card, source, victim);
    float textWidth = MeasureTextEx(assets.font, effects.c_str(), smallText, 0.0f).x;
    float width = std::max(tooltipMinWidth, textWidth + tooltipPadding);
    Rectangle box{bottomCenter.x - width / 2.0f, bottomCenter.y - tooltipHeight, width, tooltipHeight};
    DrawPanel(assets, box);

    float x = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, card.name, {x, box.y + tooltipNameY}, bigText, goldColor);
    DrawTextCenter(assets.font, TextFormat("%s - Cost %d", GetKindName(card.kind), card.cost), {x, box.y + tooltipKindY}, smallText, peachColor);
    DrawTextCenter(assets.font, effects.c_str(), {x, box.y + tooltipEffectsY}, smallText, textColor);
}

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted)
{
    const Card &card = GetCard(id);
    if (highlighted)
    {
        DrawGoldOutline(rect);
    }
    DrawPanel(assets, rect);

    Rectangle slot{rect.x + rowPadding, rect.y + (rect.height - slotSize) / 2.0f, slotSize, slotSize};
    DrawCardSlot(assets, id, slot, true, false);

    float textX = slot.x + slot.width + rowTextGap;
    std::string details = std::string(GetKindName(card.kind)) + " - " + DescribeEffects(card);
    DrawTextLeft(assets.font, card.name, {textX, rect.y + rowNameY}, bigText, highlighted ? goldColor : textColor);
    DrawTextLeft(assets.font, details.c_str(), {textX, rect.y + rowDetailsY}, smallText, peachColor);
}
