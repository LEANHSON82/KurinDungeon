#include "image.h"
#include "gameConfig.h"
#include "style.h"

static void DrawPart(Texture2D texture, Rectangle fromImage, Rectangle toScreen, Color tint)
{
    DrawTexturePro(texture, fromImage, toScreen, {0.0f, 0.0f}, 0.0f, tint);
}

void DrawIcon(Texture2D texture, Vector2 center, float scale, Color tint)
{
    float width = texture.width * scale;
    float height = texture.height * scale;
    Rectangle whole{0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    Rectangle onScreen{center.x - width / 2.0f, center.y - height / 2.0f, width, height};
    DrawPart(texture, whole, onScreen, tint);
}

void DrawBackground(Texture2D texture, Color tint)
{
    DrawIcon(texture, {screenWidth / 2.0f, screenHeight / 2.0f}, worldScale, tint);
}

static void DrawFrame(Texture2D texture, int frame, Vector2 feet, bool flip, Color tint)
{
    const float frameSize = 100.0f;
    const float feetFromTop = 57.0f;

    Rectangle fromImage{frame * frameSize, 0.0f, flip ? -frameSize : frameSize, frameSize};
    float size = frameSize * worldScale;
    Rectangle toScreen{feet.x - size / 2.0f, feet.y - feetFromTop * worldScale, size, size};
    DrawPart(texture, fromImage, toScreen, tint);
}

void DrawSprite(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color tint)
{
    DrawFrame(sheet.texture, frame, feet, flip, tint);
}

void DrawSpriteOutline(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color color)
{
    float step = worldScale;
    DrawFrame(sheet.silhouette, frame, {feet.x - step, feet.y}, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x + step, feet.y}, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x, feet.y - step}, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x, feet.y + step}, flip, color);
}

void DrawBox(const BoxImage &image, Rectangle box, Color tint)
{
    Rectangle part = image.part;
    float cx = image.cornerX;
    float cy = image.cornerY;
    float screenCx = cx * uiScale;
    float screenCy = cy * uiScale;

    float middleW = part.width - cx * 2;
    float middleH = part.height - cy * 2;
    float screenMiddleW = box.width - screenCx * 2;
    float screenMiddleH = box.height - screenCy * 2;

    float left = part.x;
    float middleX = part.x + cx;
    float right = part.x + part.width - cx;
    float top = part.y;
    float middleY = part.y + cy;
    float bottom = part.y + part.height - cy;

    float screenLeft = box.x;
    float screenMiddleX = box.x + screenCx;
    float screenRight = box.x + box.width - screenCx;
    float screenTop = box.y;
    float screenMiddleY = box.y + screenCy;
    float screenBottom = box.y + box.height - screenCy;

    DrawPart(image.texture, {left, top, cx, cy}, {screenLeft, screenTop, screenCx, screenCy}, tint);
    DrawPart(image.texture, {right, top, cx, cy}, {screenRight, screenTop, screenCx, screenCy}, tint);
    DrawPart(image.texture, {left, bottom, cx, cy}, {screenLeft, screenBottom, screenCx, screenCy}, tint);
    DrawPart(image.texture, {right, bottom, cx, cy}, {screenRight, screenBottom, screenCx, screenCy}, tint);

    DrawPart(image.texture, {middleX, top, middleW, cy}, {screenMiddleX, screenTop, screenMiddleW, screenCy}, tint);
    DrawPart(image.texture, {middleX, bottom, middleW, cy}, {screenMiddleX, screenBottom, screenMiddleW, screenCy}, tint);

    DrawPart(image.texture, {left, middleY, cx, middleH}, {screenLeft, screenMiddleY, screenCx, screenMiddleH}, tint);
    DrawPart(image.texture, {right, middleY, cx, middleH}, {screenRight, screenMiddleY, screenCx, screenMiddleH}, tint);

    DrawPart(image.texture, {middleX, middleY, middleW, middleH}, {screenMiddleX, screenMiddleY, screenMiddleW, screenMiddleH}, tint);
}

void DrawStrip(Texture2D texture, Rectangle fromImage, Rectangle toScreen, Color tint)
{
    DrawPart(texture, fromImage, toScreen, tint);
}
