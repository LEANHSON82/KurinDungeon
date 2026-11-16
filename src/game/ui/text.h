#pragma once
#include <raylib.h>

void DrawTextLeft(const Font &font, const char *text, Vector2 topLeft, float size, Color color);

void DrawTextCenter(const Font &font, const char *text, Vector2 center, float size, Color color);

void DrawTextOutlined(const Font &font, const char *text, Vector2 center, float size, Color color);
