#include "game/scenes/battle/battleLayout.h"
#include "core/components/cards.h"
#include "core/systems/battle/combatRules.h"
#include "game/components/visual.h"
#include "game/gameConfig.h"
#include "game/state/battleView.h"
#include "game/ui/cardView.h"
#include "game/ui/style.h"
#include "game/ui/widget.h"

static constexpr float feetY = 590.0f;
static constexpr float playerX = 480.0f;
static constexpr float enemyCenterX = 1440.0f;
static constexpr float enemyGap = 240.0f;
static constexpr float hudTop = 780.0f;
static constexpr float handCenterY = 935.0f;
static constexpr float slotPitch = 132.0f;
static constexpr float slotRaise = 18.0f;

Vector2 GetPlayerFeet()
{
    return {playerX, feetY};
}

Vector2 GetEnemyFeet(int slot, int enemiesInBattle)
{
    float offset = slot - (enemiesInBattle - 1) / 2.0f;
    return {enemyCenterX + offset * enemyGap, feetY};
}

Vector2 GetInfoPoint(Vector2 feet)
{
    return {feet.x, feet.y + 46.0f};
}

Vector2 GetIntentPoint(Vector2 feet)
{
    return {feet.x, feet.y - 180.0f};
}

Rectangle GetActorRect(Vector2 feet)
{
    return {feet.x - 100.0f, feet.y - 200.0f, 200.0f, 220.0f};
}

Rectangle GetHudRect()
{
    return {-12.0f, hudTop, screenWidth + 24.0f, screenHeight - hudTop + 12.0f};
}

Vector2 GetHealthCenter()
{
    return {190.0f, handCenterY};
}

Vector2 GetEnergyCenter()
{
    return {screenWidth - 190.0f, handCenterY};
}

Rectangle GetCardRect(int index, int handCount, bool raised)
{
    float total = (handCount - 1) * slotPitch + slotSize;
    float x = (screenWidth - total) / 2.0f + index * slotPitch;
    float y = handCenterY - slotSize / 2.0f - (raised ? slotRaise : 0.0f);
    return {x, y, slotSize, slotSize};
}

Rectangle GetEndTurnButton()
{
    return {screenWidth - 300.0f, hudTop - 90.0f, 252.0f, buttonHeight};
}

Rectangle GetContinueButton()
{
    return CenteredRect(500.0f, 336.0f, buttonHeight);
}

const Card *GetSelectedCard(const ecs::Registry &registry)
{
    ecs::Entity card = registry.Resource<BattleUi>().selectedCard;
    if (!registry.Valid(card))
    {
        return nullptr;
    }
    return &GetCard(registry.Get<DeckCard>(card).id);
}

ecs::Entity GetCardUnderMouse(const std::vector<ecs::Entity> &hand, ecs::Entity selectedCard)
{
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        if (IsHovered(GetCardRect(i, count, hand[i] == selectedCard)))
        {
            return hand[i];
        }
    }
    return ecs::nullEntity;
}

ecs::Entity GetEnemyUnderMouse(const ecs::Registry &registry)
{
    for (ecs::Entity enemy : GetEnemiesInOrder(registry))
    {
        if (IsEnemyAlive(registry, enemy) && IsHovered(GetActorRect(registry.Get<Position>(enemy).value)))
        {
            return enemy;
        }
    }
    return ecs::nullEntity;
}
