#pragma once
#include <raylib.h>
#include "game/assets/assets.h"
#include "game/ui/style.h"

Rectangle CenteredRect(float y, float width, float height);

// Call once per frame before any input checks.
void UpdatePointer();
// Forget the current press, so a click that started in another scene does not count.
void ResetPointer();

bool IsHovered(Rectangle rect);
// True when the left button is released over rect after being pressed over rect.
bool IsClicked(Rectangle rect);
bool IsConfirmPressed(Rectangle button);

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold);
void DrawPanel(const Assets &assets, Rectangle rect);
void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp);
void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint);
void DrawOrb(const Assets &assets, Vector2 center, Texture2D fill, Texture2D frame, bool frameOnRight, int value, int maxValue);

void DrawScreenShade(unsigned char alpha);
void DrawSceneBackdrop(const Assets &assets, Texture2D background, unsigned char shade, const char *title = nullptr, float titleY = headerY);
void DrawBottomShade(float top);
