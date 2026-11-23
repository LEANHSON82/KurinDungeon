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

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected)
{
    const Card &card = GetCard(id);
    Color tint = playable ? WHITE : unplayableTint;
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f};

    if (selected)
    {
        Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawIcon(assets.getTexture(card.art), center, slotScale, tint);
    DrawNumber(assets, card.cost, {rect.x + 12.0f, rect.y + 12.0f}, detailScale, playable ? WHITE : GRAY);
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
    DrawTextOutlined(font, card.name, {x, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(font, TextFormat("%s - Cost %d", GetKindName(card.kind), card.cost), {x, box.y + 100.0f}, smallText, peachColor);
    DrawTextCenter(font, effects.c_str(), {x, box.y + 144.0f}, smallText, textColor);
}

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted)
{
    const Card &card = GetCard(id);
    const Font &font = assets.getFont("Main");
    if (highlighted)
    {
        Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawPanel(assets, rect);

    Rectangle slot{rect.x + 30.0f, rect.y + (rect.height - slotSize) / 2.0f, slotSize, slotSize};
    DrawCardSlot(assets, id, slot, true, false);

    float textX = slot.x + slot.width + 36.0f;
    std::string details = std::string(GetKindName(card.kind)) + " - " + DescribeEffects(card);
    DrawTextLeft(font, card.name, {textX, rect.y + 24.0f}, bigText, highlighted ? goldColor : textColor);
    DrawTextLeft(font, details.c_str(), {textX, rect.y + 88.0f}, smallText, peachColor);
}
