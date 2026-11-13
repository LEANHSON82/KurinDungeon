#pragma once
#include <raylib.h>
#include "assets/assets.h"

void DrawIcon(Texture2D texture, Vector2 center, float scale, Color tint);

void DrawBackground(Texture2D texture, Color tint);

void DrawSprite(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color tint);

void DrawSpriteOutline(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color color);

void DrawBox(const BoxImage &image, Rectangle box, Color tint);

void DrawStrip(Texture2D texture, Rectangle fromImage, Rectangle toScreen, Color tint);
