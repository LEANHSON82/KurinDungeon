#include "cardView.h"
#include "cards/cardDatabase.h"
#include "ui/image.h"
#include "ui/style.h"
#include "ui/text.h"
#include "ui/widget.h"

static const float tooltipWidth = 420.0f;
static const float tooltipHeight = 192.0f;

static const char *GetKindName(const Card &card)
{
    return card.target == Target::Self ? "Skill" : "Attack";
}

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected)
{
    const Card &card = GetCard(id);
    Color tint = playable ? WHITE : Color{100, 95, 110, 255};
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f};

    if (selected)
    {
        Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawIcon(GetCardArt(assets, id), center, slotScale, tint);

    DrawNumber(assets, card.cost, {rect.x + 12.0f, rect.y + 12.0f}, detailScale, playable ? WHITE : GRAY);
}

void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter)
{
    const Card &card = GetCard(id);
    Rectangle box{bottomCenter.x - tooltipWidth / 2.0f, bottomCenter.y - tooltipHeight, tooltipWidth, tooltipHeight};
    DrawPanel(assets, box);

    float x = box.x + box.width / 2.0f;
    DrawTextOutlined(assets.font, card.name, {x, box.y + 44.0f}, bigText, goldColor);
    DrawTextCenter(assets.font, TextFormat("%s - Cost %d", GetKindName(card), card.cost), {x, box.y + 100.0f}, smallText, peachColor);
    DrawTextCenter(assets.font, DescribeEffects(card).c_str(), {x, box.y + 144.0f}, smallText, textColor);
}

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted)
{
    const Card &card = GetCard(id);
    if (highlighted)
    {
        Rectangle outline{rect.x - uiScale, rect.y - uiScale, rect.width + uiScale * 2, rect.height + uiScale * 2};
        DrawRectangleRec(outline, goldColor);
    }
    DrawPanel(assets, rect);

    Rectangle slot{rect.x + 30.0f, rect.y + (rect.height - slotSize) / 2.0f, slotSize, slotSize};
    DrawCardSlot(assets, id, slot, true, false);

    float textX = slot.x + slot.width + 36.0f;
    DrawTextLeft(assets.font, card.name, {textX, rect.y + 24.0f}, bigText, highlighted ? goldColor : textColor);
    DrawTextLeft(assets.font, TextFormat("%s - %s", GetKindName(card), DescribeEffects(card).c_str()), {textX, rect.y + 88.0f}, smallText, peachColor);
}
