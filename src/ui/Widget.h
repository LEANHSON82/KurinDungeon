#pragma once
#include <raylib.h>
#include "engine/Assets.h"
#include "ui/Style.h"

Rectangle CenteredRect(float y, float width, float height);
bool IsHovered(Rectangle rect);

// held: the mouse button went down on this button and is still down (see Scene::isHeld).
void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold, bool held);
void DrawPanel(const Assets &assets, Rectangle rect);
void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp);
void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint);
void DrawOrb(const Assets &assets, Vector2 center, const char *fill, const char *frame, bool frameOnRight, int value, int maxValue);

void DrawScreenShade(unsigned char alpha);
void DrawBottomShade(float top);
void DrawSceneBackdrop(const Assets &assets, const char *background, unsigned char shade, const char *title = nullptr, float titleY = headerY);
