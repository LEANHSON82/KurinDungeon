#pragma once
#include <algorithm>
#include <raylib.h>
#include "engine/GameConfig.h"
#include "ui/CardView.h"
#include "ui/Widget.h"

// Where things sit on the battle screen. Input (what did the mouse hit?) and drawing use
// the same functions, so what you see is what you click.

inline constexpr float feetY = 590.0f;
inline constexpr float playerX = 480.0f;
inline constexpr float enemyCenterX = 1440.0f;
inline constexpr float enemyGap = 240.0f;
inline constexpr float hudTop = 780.0f;
inline constexpr float handCenterY = 935.0f;
inline constexpr float slotPitch = 132.0f;
inline constexpr float slotRaise = 18.0f;
inline constexpr float handAreaWidth = 880.0f; // room for the hand between the draw and discard piles

inline Vector2 EnemyFeet(int slot, int enemiesInBattle)
{
    float offset = slot - (enemiesInBattle - 1) / 2.0f;
    return {enemyCenterX + offset * enemyGap, feetY};
}

inline Rectangle ActorRect(Vector2 feet)
{
    return {feet.x - 100.0f, feet.y - 200.0f, 200.0f, 220.0f};
}

inline Rectangle HudRect()
{
    return {-12.0f, hudTop, screenWidth + 24.0f, screenHeight - hudTop + 12.0f};
}

inline Vector2 HealthCenter()
{
    return {190.0f, handCenterY};
}

inline Vector2 EnergyCenter()
{
    return {screenWidth - 190.0f, handCenterY};
}

inline Rectangle CardRect(int index, int handCount, bool raised)
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

inline Rectangle EndTurnButton()
{
    return {screenWidth - 300.0f, hudTop - 90.0f, 252.0f, buttonHeight};
}

inline Rectangle ContinueButton()
{
    return CenteredRect(500.0f, 336.0f, buttonHeight);
}
