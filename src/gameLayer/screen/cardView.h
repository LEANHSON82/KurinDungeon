#pragma once
#include <raylib.h>
#include "assets/assets.h"
#include "cards/card.h"

const float slotScale = 6.0f;
const float slotSize = 16.0f * slotScale;
const float rowHeight = 150.0f;

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected);

void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter);

void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted);
