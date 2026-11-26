#include "text.h"
#include "style.h"

const float headingTightening = 2.0f;

static void DrawSpaced(const Font &font, const char *text, Vector2 center, float size, float spacing, Color color)
{
    Vector2 textSize = MeasureTextEx(font, text, size, spacing);
    Vector2 topLeft{center.x - textSize.x / 2.0f, center.y - textSize.y / 2.0f};
    DrawTextEx(font, text, topLeft, size, spacing, color);
}

static void DrawSpacedOutlined(const Font &font, const char *text, Vector2 center, float size, float spacing, Color color)
{
    float pixel = size / font.baseSize;

    DrawSpaced(font, text, {center.x - pixel, center.y}, size, spacing, inkColor);
    DrawSpaced(font, text, {center.x + pixel, center.y}, size, spacing, inkColor);
    DrawSpaced(font, text, {center.x, center.y - pixel}, size, spacing, inkColor);
    DrawSpaced(font, text, {center.x, center.y + pixel}, size, spacing, inkColor);
    DrawSpaced(font, text, center, size, spacing, color);
}

void DrawTextLeft(const Font &font, const char *text, Vector2 topLeft, float size, Color color)
{
    DrawTextEx(font, text, topLeft, size, 0.0f, color);
}

void DrawTextCenter(const Font &font, const char *text, Vector2 center, float size, Color color)
{
    DrawSpaced(font, text, center, size, 0.0f, color);
}

void DrawTextOutlined(const Font &font, const char *text, Vector2 center, float size, Color color)
{
    DrawSpacedOutlined(font, text, center, size, 0.0f, color);
}

void DrawHeading(const Font &titleFont, const char *text, Vector2 center, float size, Color color)
{
    float pixel = size / titleFont.baseSize;
    DrawSpacedOutlined(titleFont, text, center, size, -headingTightening * pixel, color);
}
