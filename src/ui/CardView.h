#pragma once
#include <functional>
#include <raylib.h>
#include "data/Cards.h"
#include "engine/Assets.h"

inline constexpr float slotScale = 6.0f;
inline constexpr float slotSize = 16.0f * slotScale;
inline constexpr float rowHeight = 150.0f;

void DrawCardSlot(const Assets &assets, CardId id, Rectangle rect, bool playable, bool selected);
// damage (optional) turns base damage into the real number shown in the text.
void DrawCardTooltip(const Assets &assets, CardId id, Vector2 bottomCenter, const std::function<int(int)> &damage = nullptr);
void DrawCardRow(const Assets &assets, CardId id, Rectangle rect, bool highlighted);
