#include <algorithm>
#include "battleLayout.h"
#include "gameConfig.h"
#include "combat/battleEvent.h"
#include "screen/cardView.h"
#include "screen/ui/widget.h"

const float feetY = 590.0f;
const float playerX = 480.0f;
const float enemyCenterX = 1440.0f;
const float enemyGap = 240.0f;
const float handCenterY = 935.0f;
const float slotPitch = 132.0f;
const float slotRaise = 18.0f;
const float handAreaWidth = 880.0f;

Vector2 GetActorFeet(int actor, int enemyCount)
{
    if (actor == playerActor)
    {
        return {playerX, feetY};
    }
    float offset = actor - (enemyCount - 1) / 2.0f;
    return {enemyCenterX + offset * enemyGap, feetY};
}

Rectangle GetActorRect(int actor, int enemyCount)
{
    Vector2 feet = GetActorFeet(actor, enemyCount);
    return {feet.x - 100.0f, feet.y - 200.0f, 200.0f, 220.0f};
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
    float pitch = slotPitch;
    if (handCount > 1)
    {
        pitch = std::min(slotPitch, (handAreaWidth - slotSize) / (handCount - 1));
    }
    float total = (handCount - 1) * pitch + slotSize;
    float x = (screenWidth - total) / 2.0f + index * pitch;
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

int GetCardUnderMouse(const Battle &battle, int selectedCard)
{
    int count = static_cast<int>(battle.GetDeck().GetHand().size());
    for (int i = count - 1; i >= 0; i--)
    {
        if (IsHovered(GetCardRect(i, count, i == selectedCard)))
        {
            return i;
        }
    }
    return -1;
}

int GetEnemyUnderMouse(const Battle &battle)
{
    int count = static_cast<int>(battle.GetEnemies().size());
    for (int i = 0; i < count; i++)
    {
        if (battle.IsEnemyAlive(i) && IsHovered(GetActorRect(i, count)))
        {
            return i;
        }
    }
    return -1;
}
