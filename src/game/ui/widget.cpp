#include "game/ui/widget.h"
#include <algorithm>
#include "game/gameConfig.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"

Rectangle CenteredRect(float y, float width, float height)
{
    return {screenWidth / 2.0f - width / 2.0f, y, width, height};
}

namespace
{
struct Pointer
{
    Vector2 pressedAt{};
    bool pressed = false;
};

Pointer pointer;

bool IsPressStartedIn(Rectangle rect)
{
    return pointer.pressed && CheckCollisionPointRec(pointer.pressedAt, rect);
}
}

void UpdatePointer()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        pointer.pressedAt = GetMousePosition();
        pointer.pressed = true;
    }
}

void ResetPointer()
{
    pointer.pressed = false;
}

bool IsHovered(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

bool IsClicked(Rectangle rect)
{
    return IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && IsHovered(rect) && IsPressStartedIn(rect);
}

bool IsConfirmPressed(Rectangle button)
{
    return IsKeyPressed(KEY_ENTER) || IsClicked(button);
}

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold)
{
    const BoxImage *image = gold ? &assets.goldButtonNormal : &assets.buttonNormal;
    if (IsHovered(rect))
    {
        bool held = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsPressStartedIn(rect);
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
    const float height = 72.0f;
    Rectangle frame{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawBox(assets.healthFrame, frame, WHITE);

    Rectangle channel{frame.x + 3 * uiScale, frame.y + 3 * uiScale, frame.width - 6 * uiScale, frame.height - 7 * uiScale};
    float filled = maxHp > 0 ? channel.width * std::clamp(hp, 0, maxHp) / maxHp : 0.0f;
    DrawStrip(assets.healthFill, {5, 7, 38, 1}, {channel.x, channel.y, filled, uiScale}, WHITE);
    DrawStrip(assets.healthFill, {5, 8, 38, 1}, {channel.x, channel.y + uiScale, filled, channel.height - uiScale}, WHITE);

    Vector2 textCenter{center.x, channel.y + channel.height / 2.0f - smallTextNudge};
    DrawTextOutlined(assets.font, TextFormat("%d/%d", hp, maxHp), textCenter, smallText, textColor);
}

void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint)
{
    const char *text = TextFormat("%d", std::max(0, value));
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
        DrawTextOutlined(assets.font, title, {screenWidth / 2.0f, titleY}, titleText, goldColor);
    }
}
