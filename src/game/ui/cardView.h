#pragma once
#include <raylib.h>
#include "core/data/cards.h"
#include "game/assets/assets.h"

inline constexpr float slotScale = 6.0f;
inline constexpr float slotSize = 16.0f * slotScale;
inline constexpr float rowHeight = 150.0f;

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected);

void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter);

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted);
