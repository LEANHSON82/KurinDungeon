#pragma once
#include <raylib.h>
#include "assets/assets.h"
#include "style.h"

Rectangle CenteredRect(float y, float width, float height);
bool IsHovered(Rectangle rect);
bool IsClicked(Rectangle rect);

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold);
void DrawPanel(const Assets &assets, Rectangle rect);
void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp);
void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint);
void DrawOrb(const Assets &assets, Vector2 center, Texture2D fill, Texture2D frame, bool frameOnRight, int value, int maxValue);

void DrawScreenShade(unsigned char alpha);
void DrawBottomShade(float top);
void DrawSceneBackdrop(const Assets &assets, Texture2D background, unsigned char shade, const char *title = nullptr, float titleY = headerY);
