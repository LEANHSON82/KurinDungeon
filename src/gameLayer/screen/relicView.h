#pragma once
#include <vector>
#include <raylib.h>
#include "assets/assets.h"
#include "relics/relic.h"

void DrawRelicBar(const Assets &assets, const std::vector<RelicId> &relics, Vector2 topLeft);
void DrawRelicRow(const Assets &assets, RelicId id, Rectangle rect, bool highlighted);
void DrawGoldAmount(const Assets &assets, const char *text, Vector2 center);
