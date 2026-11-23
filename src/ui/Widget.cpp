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

void DrawButton(const Assets &assets, Rectangle rect, const char *label, ButtonStyle style, bool held)
{
    bool gold = style == ButtonStyle::Gold;
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
    const float height = 56.0f;
    Rectangle frame{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawBox(assets.getBox("HealthFrame"), frame, WHITE);

    // The channel is the hollow inside of the frame art, whose border is 3 art pixels thick
    // (4 at the bottom).
    const float border = 3 * uiScale;
    const float bottomBorder = 4 * uiScale;
    Rectangle channel{frame.x + border, frame.y + border, frame.width - border * 2, frame.height - border - bottomBorder};
    float filled = maxHp > 0 ? channel.width * std::clamp(hp, 0, maxHp) / maxHp : 0.0f;

    // The fill art is two rows of pixels: a light row on top of a plain one. The light row
    // stays one art pixel high, the plain row stretches over the rest of the channel.
    Texture2D fill = assets.getTexture("HealthFill");
    const Rectangle lightRow{5, 7, 38, 1};
    const Rectangle plainRow{5, 8, 38, 1};
    DrawPart(fill, lightRow, {channel.x, channel.y, filled, uiScale}, WHITE);
    DrawPart(fill, plainRow, {channel.x, channel.y + uiScale, filled, channel.height - uiScale}, WHITE);

    Vector2 textCenter{center.x, channel.y + channel.height / 2.0f - smallTextNudge};
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d/%d", hp, maxHp), textCenter, smallText, textColor);
}

void DrawNumber(const Assets &assets, int value, Vector2 center, float scale, Color tint)
{
    std::string text = std::to_string(std::max(0, value));
    int count = static_cast<int>(text.size());
    const float digitPitch = 11.0f; // art pixels from one digit to the next
    float step = digitPitch * scale;
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
    // The orb art is 48 x 48 pixels and its liquid fills the rows from 5 down to 43.
    const float artSize = 48.0f;
    const float liquidTop = 5.0f;
    const float liquidHeight = 38.0f;
    const float frameShift = 16.0f; // the decorated frame hangs off one side of the orb
    const float size = artSize * detailScale;

    // Only the lower part of the liquid is drawn: the emptier the orb, the lower it starts.
    float full = maxValue > 0 ? std::clamp(static_cast<float>(value) / maxValue, 0.0f, 1.0f) : 0.0f;
    float top = liquidTop + liquidHeight * (1.0f - full);

    DrawIcon(assets.getTexture("OrbEmpty"), center, detailScale, WHITE);
    Rectangle fromImage{0.0f, top, artSize, artSize - top};
    Rectangle toScreen{center.x - size / 2.0f, center.y - size / 2.0f + top * detailScale, size, (artSize - top) * detailScale};
    DrawPart(assets.getTexture(fill), fromImage, toScreen, WHITE);

    float frameOffset = (frameOnRight ? frameShift : -frameShift) * detailScale;
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
        DrawHeading(assets.getFont("Title"), title, {screenWidth / 2.0f, titleY}, headingText, goldColor);
    }
}
