#pragma once
#include <raylib.h>
#include "assets/assets.h"

bool IsHovered(Rectangle rect);
bool IsClicked(Rectangle rect);

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold);
void DrawPanel(const Assets &assets, Rectangle rect);
void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp);
void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint);
void DrawOrb(const Assets &assets, Vector2 center, Texture2D fill, Texture2D frame, bool frameOnRight, int value, int maxValue);

void DrawScreenShade(unsigned char alpha);
void DrawBottomShade(float top);
