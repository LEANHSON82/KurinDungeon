#include "ui/Widget.h"
#include <algorithm>
#include <string>
#include "engine/GameConfig.h"
#include "ui/Image.h"
#include "ui/Text.h"

Rectangle CenteredRect(float y, float width, float height)
{
    return {screenWidth / 2.0f - width / 2.0f, y, width, height};
}

bool IsHovered(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

void DrawButton(const Assets &assets, Rectangle rect, const char *label, bool gold, bool held)
{
    std::string box = gold ? "GoldButton" : "Button";
    if (IsHovered(rect))
    {
        box += held ? "Pressed" : "Hover";
    }

    DrawBox(assets.getBox(box), rect, WHITE);
    Vector2 center{rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f - bigTextNudge};
    DrawTextOutlined(assets.getFont("Main"), label, center, bigText, gold ? textColor : peachColor);
}

void DrawPanel(const Assets &assets, Rectangle rect)
{
    const BoxImage &box = assets.getBox("DarkBox");
    DrawBox(box, {rect.x, rect.y + uiScale, rect.width, rect.height}, Fade(shadeColor, 0.6f));
    DrawBox(box, rect, panelTint);
}

void DrawHealthBar(const Assets &assets, Vector2 center, float width, int hp, int maxHp)
{
    const float height = 72.0f;
    Rectangle frame{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawBox(assets.getBox("HealthFrame"), frame, WHITE);

    Texture2D fill = assets.getTexture("HealthFill");
    Rectangle channel{frame.x + 3 * uiScale, frame.y + 3 * uiScale, frame.width - 6 * uiScale, frame.height - 7 * uiScale};
    float filled = maxHp > 0 ? channel.width * std::clamp(hp, 0, maxHp) / maxHp : 0.0f;
    DrawStrip(fill, {5, 7, 38, 1}, {channel.x, channel.y, filled, uiScale}, WHITE);
    DrawStrip(fill, {5, 8, 38, 1}, {channel.x, channel.y + uiScale, filled, channel.height - uiScale}, WHITE);

    Vector2 textCenter{center.x, channel.y + channel.height / 2.0f - smallTextNudge};
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d/%d", hp, maxHp), textCenter, smallText, textColor);
}

void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint)
{
    std::string text = std::to_string(std::max(0, value));
    int count = static_cast<int>(text.size());
    float step = 11.0f * scale;
    float x = center.x - step * (count - 1) / 2.0f;
    for (int i = 0; i < count; i++)
    {
        std::string digit = "Digit";
        digit += text[i];
        DrawIcon(assets.getTexture(digit), {x + i * step, center.y}, scale, tint);
    }
}

void DrawOrb(const Assets &assets, Vector2 center, const char *fill, const char *frame, bool frameOnRight, int value, int maxValue)
{
    const float size = 48.0f * detailScale;
    float full = maxValue > 0 ? std::clamp(static_cast<float>(value) / maxValue, 0.0f, 1.0f) : 0.0f;
    float top = 5.0f + 38.0f * (1.0f - full);

    DrawIcon(assets.getTexture("OrbEmpty"), center, detailScale, WHITE);
    Rectangle fromImage{0.0f, top, 48.0f, 48.0f - top};
    Rectangle toScreen{center.x - size / 2.0f, center.y - size / 2.0f + top * detailScale, size, (48.0f - top) * detailScale};
    DrawStrip(assets.getTexture(fill), fromImage, toScreen, WHITE);

    float frameOffset = (frameOnRight ? 16.0f : -16.0f) * detailScale;
    DrawIcon(assets.getTexture(frame), {center.x + frameOffset, center.y}, detailScale, WHITE);
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d/%d", value, maxValue), {center.x, center.y - bigTextNudge}, bigText, textColor);
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

void DrawSceneBackdrop(const Assets &assets, const char *background, unsigned char shade, const char *title, float titleY)
{
    DrawBackground(assets.getTexture(background), WHITE);
    DrawScreenShade(shade);
    if (title != nullptr)
    {
        DrawTextOutlined(assets.getFont("Main"), title, {screenWidth / 2.0f, titleY}, titleText, goldColor);
    }
}
