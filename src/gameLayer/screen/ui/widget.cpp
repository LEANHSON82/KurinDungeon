#include "widget.h"
#include <algorithm>
#include <string>
#include "gameConfig.h"
#include "image.h"
#include "text.h"

Rectangle CenteredRect(float y, float width, float height)
{
    return {screenWidth / 2.0f - width / 2.0f, y, width, height};
}

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
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f - bigTextNudge};
    DrawTextOutlined(assets.font, label, center, bigText, gold ? textColor : peachColor);
}

void DrawPanel(const Assets &assets, Rectangle rect)
{
    DrawBox(assets.darkBox, {rect.x, rect.y + uiScale, rect.width, rect.height}, Fade(shadeColor, 0.6f));
    DrawBox(assets.darkBox, rect, panelTint);
}

void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp)
{
    const float height = 56.0f;
    Rectangle frame{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawBox(assets.healthFrame, frame, WHITE);

    const float border = 3 * uiScale;
    const float bottomBorder = 4 * uiScale;
    Rectangle channel{frame.x + border, frame.y + border, frame.width - border * 2, frame.height - border - bottomBorder};
    float filled = maxHp > 0 ? channel.width * std::clamp(hp, 0, maxHp) / maxHp : 0.0f;

    const Rectangle lightRow{5, 7, 38, 1};
    const Rectangle plainRow{5, 8, 38, 1};
    DrawPart(assets.healthFill, lightRow, {channel.x, channel.y, filled, uiScale}, WHITE);
    DrawPart(assets.healthFill, plainRow, {channel.x, channel.y + uiScale, filled, channel.height - uiScale}, WHITE);

    Vector2 textCenter{center.x, channel.y + channel.height / 2.0f - smallTextNudge};
    DrawTextOutlined(assets.font, TextFormat("%d/%d", hp, maxHp), textCenter, smallText, textColor);
}

void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint)
{
    std::string text = std::to_string(std::max(0, value));
    int count = static_cast<int>(text.size());
    const float digitPitch = 11.0f;
    float step = digitPitch * scale;
    float x = center.x - step * (count - 1) / 2.0f;
    for (int i = 0; i < count; i++)
    {
        int digit = text[i] - '0';
        DrawIcon(assets.digits[digit], {x + i * step, center.y}, scale, tint);
    }
}

void DrawOrb(const Assets &assets, Vector2 center, Texture2D fill, Texture2D frame, bool frameOnRight, int value, int maxValue)
{
    const float artSize = 48.0f;
    const float liquidTop = 5.0f;
    const float liquidHeight = 38.0f;
    const float frameShift = 16.0f;
    const float size = artSize * detailScale;

    float full = maxValue > 0 ? std::clamp(static_cast<float>(value) / maxValue, 0.0f, 1.0f) : 0.0f;
    float top = liquidTop + liquidHeight * (1.0f - full);

    DrawIcon(assets.orbEmpty, center, detailScale, WHITE);
    Rectangle fromImage{0.0f, top, artSize, artSize - top};
    Rectangle toScreen{center.x - size / 2.0f, center.y - size / 2.0f + top * detailScale, size, (artSize - top) * detailScale};
    DrawPart(fill, fromImage, toScreen, WHITE);

    float frameOffset = (frameOnRight ? frameShift : -frameShift) * detailScale;
    DrawIcon(frame, {center.x + frameOffset, center.y}, detailScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d/%d", value, maxValue), {center.x, center.y - bigTextNudge}, bigText, textColor);
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

void DrawSceneBackdrop(const Assets &assets, Texture2D background, unsigned char shade, const char *title, float titleY)
{
    DrawBackground(background, WHITE);
    DrawScreenShade(shade);
    if (title != nullptr)
    {
        DrawHeading(assets.titleFont, title, {screenWidth / 2.0f, titleY}, headingText, goldColor);
    }
}
