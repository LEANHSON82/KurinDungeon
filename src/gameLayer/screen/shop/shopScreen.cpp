#include <raylib.h>
#include "shopScreen.h"
#include "gameConfig.h"
#include "cards/cardDatabase.h"
#include "screen/cardView.h"
#include "screen/relicView.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float rowWidth = 760.0f;
const float rowGap = 40.0f;
const float firstRowY = 280.0f;
const float leftColumnX = 160.0f;
const float rightColumnX = 1000.0f;
const int rowsPerColumn = 3;
const float goldY = 210.0f;
const float priceInset = 90.0f;

static Rectangle GetItemRect(int index)
{
    int column = index / rowsPerColumn;
    int row = index % rowsPerColumn;
    float x = column == 0 ? leftColumnX : rightColumnX;
    return {x, firstRowY + row * (rowHeight + rowGap), rowWidth, rowHeight};
}

static Rectangle GetLeaveButton()
{
    return CenteredRect(900.0f, 360.0f, buttonHeight);
}

static int GetItemCount(const ShopStock &shop)
{
    return static_cast<int>(shop.cards.size()) + (shop.hasRelic ? 1 : 0);
}

static bool IsRelicItem(const ShopStock &shop, int index)
{
    return index == static_cast<int>(shop.cards.size());
}

static bool IsSold(const ShopStock &shop, int index)
{
    if (IsRelicItem(shop, index))
    {
        return shop.relicSold;
    }
    return shop.cardSold[index];
}

static int GetPrice(const ShopStock &shop, int index)
{
    if (IsRelicItem(shop, index))
    {
        return relicPrice;
    }
    return GetCardPrice(shop.cards[index]);
}

ScreenAction UpdateShopScreen(const ShopStock &shop, int gold, int &chosenItem)
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetLeaveButton()))
    {
        return ScreenAction::ShopLeft;
    }

    for (int i = 0; i < GetItemCount(shop); i++)
    {
        bool canBuy = !IsSold(shop, i) && gold >= GetPrice(shop, i);
        if (canBuy && IsClicked(GetItemRect(i)))
        {
            chosenItem = i;
            return ScreenAction::ItemBought;
        }
    }
    return ScreenAction::None;
}

static void DrawPrice(const ShopStock &shop, int index, int gold, Rectangle rect, const Assets &assets)
{
    Vector2 center{rect.x + rect.width - priceInset, rect.y + rect.height / 2.0f};
    if (IsSold(shop, index))
    {
        DrawTextOutlined(assets.font, "Sold", {center.x, center.y - bigTextNudge}, bigText, peachColor);
        return;
    }
    int price = GetPrice(shop, index);
    DrawGoldAmount(assets, TextFormat("%d", price), center);
    if (gold < price)
    {
        DrawRectangleRec(rect, Fade(shadeColor, 0.45f));
    }
}

void DrawShopScreen(const ShopStock &shop, int gold, const Assets &assets)
{
    DrawPlainBackdrop(assets, "Shop");
    DrawGoldAmount(assets, TextFormat("%d", gold), {screenWidth / 2.0f, goldY});

    for (int i = 0; i < GetItemCount(shop); i++)
    {
        Rectangle rect = GetItemRect(i);
        bool highlighted = IsHovered(rect) && !IsSold(shop, i) && gold >= GetPrice(shop, i);
        if (IsRelicItem(shop, i))
        {
            DrawRelicRow(assets, shop.relic, rect, highlighted);
        }
        else
        {
            DrawCardRow(assets, shop.cards[i], rect, highlighted);
        }
        if (IsSold(shop, i))
        {
            DrawRectangleRec(rect, Fade(shadeColor, 0.6f));
        }
        DrawPrice(shop, i, gold, rect, assets);
    }

    DrawButton(assets, GetLeaveButton(), "Leave", true);
}
