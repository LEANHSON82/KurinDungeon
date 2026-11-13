#include "text.h"
#include "style.h"

void DrawTextLeft(const Font &font, const char *text, Vector2 topLeft, float size, Color color)
{
    DrawTextEx(font, text, topLeft, size, 0.0f, color);
}

void DrawTextCenter(const Font &font, const char *text, Vector2 center, float size, Color color)
{
    Vector2 textSize = MeasureTextEx(font, text, size, 0.0f);
    Vector2 topLeft{center.x - textSize.x / 2.0f, center.y - textSize.y / 2.0f};
    DrawTextLeft(font, text, topLeft, size, color);
}

void DrawTextOutlined(const Font &font, const char *text, Vector2 center, float size, Color color)
{
    float pixel = size / font.baseSize;

    DrawTextCenter(font, text, {center.x - pixel, center.y}, size, inkColor);
    DrawTextCenter(font, text, {center.x + pixel, center.y}, size, inkColor);
    DrawTextCenter(font, text, {center.x, center.y - pixel}, size, inkColor);
    DrawTextCenter(font, text, {center.x, center.y + pixel}, size, inkColor);
    DrawTextCenter(font, text, center, size, color);
}
