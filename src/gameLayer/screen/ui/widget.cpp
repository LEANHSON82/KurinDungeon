#include "widget.h"
#include "gameConfig.h"
#include "image.h"
#include "style.h"
#include "text.h"

bool IsHovered(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool IsClicked(Rectangle rect)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsHovered(rect);
}

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold)
{
    const BoxImage *image = gold ? &assets.goldButtonNormal : &assets.buttonNormal;
    if (IsHovered(rect))
    {
        bool held = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        if (gold)
        {
            image = held ? &assets.goldButtonPressed : &assets.goldButtonHover;
        }
        else
        {
            image = held ? &assets.buttonPressed : &assets.buttonHover;
        }
    }

    DrawBox(*image, rect, WHITE);
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f - 6.0f};
    DrawTextOutlined(assets.font, label, center, bigText, gold ? textColor : peachColor);
}

void DrawPanel(const Assets &assets, Rectangle rect)
{
    DrawBox(assets.darkBox, {rect.x, rect.y + uiScale, rect.width, rect.height}, Fade(shadeColor, 0.6f));
    DrawBox(assets.darkBox, rect, panelTint);
}

void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp)
{
    const float height = 72.0f;
    Rectangle frame{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawBox(assets.healthFrame, frame, WHITE);

    Rectangle channel{frame.x + 3 * uiScale, frame.y + 3 * uiScale, frame.width - 6 * uiScale, frame.height - 7 * uiScale};
    float filled = channel.width * hp / maxHp;
    DrawStrip(assets.healthFill, {5, 7, 38, 1}, {channel.x, channel.y, filled, uiScale}, WHITE);
    DrawStrip(assets.healthFill, {5, 8, 38, 1}, {channel.x, channel.y + uiScale, filled, channel.height - uiScale}, WHITE);

    Vector2 textCenter{center.x, channel.y + channel.height / 2.0f - 4.0f};
    DrawTextOutlined(assets.font, TextFormat("%d/%d", hp, maxHp), textCenter, smallText, textColor);
}

void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint)
{
    const char *text = TextFormat("%d", value);
    int count = TextLength(text);
    float step = 11.0f * scale;
    float x = center.x - step * (count - 1) / 2.0f;
    for (int i = 0; i < count; i++)
    {
        DrawIcon(assets.digits[text[i] - '0'], {x + i * step, center.y}, scale, tint);
    }
}

void DrawOrb(const Assets &assets, Vector2 center, Texture2D fill, Texture2D frame, bool frameOnRight, int value, int maxValue)
{
    const float size = 48.0f * detailScale;
    float full = maxValue > 0 ? static_cast<float>(value) / maxValue : 0.0f;
    float top = 5.0f + 38.0f * (1.0f - full);

    DrawIcon(assets.orbEmpty, center, detailScale, WHITE);
    Rectangle fromImage{0.0f, top, 48.0f, 48.0f - top};
    Rectangle toScreen{center.x - size / 2.0f, center.y - size / 2.0f + top * detailScale, size, (48.0f - top) * detailScale};
    DrawStrip(fill, fromImage, toScreen, WHITE);

    float frameOffset = (frameOnRight ? 16.0f : -16.0f) * detailScale;
    DrawIcon(frame, {center.x + frameOffset, center.y}, detailScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d/%d", value, maxValue), {center.x, center.y - 6.0f}, bigText, textColor);
}

void DrawScreenShade(unsigned char alpha)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{shadeColor.r, shadeColor.g, shadeColor.b, alpha});
}

void DrawBottomShade(float top)
{
    Color clear{shadeColor.r, shadeColor.g, shadeColor.b, 0};
    Color dark{shadeColor.r, shadeColor.g, shadeColor.b, 210};
    DrawRectangleGradientV(0, static_cast<int>(top), screenWidth, static_cast<int>(screenHeight - top), clear, dark);
}
