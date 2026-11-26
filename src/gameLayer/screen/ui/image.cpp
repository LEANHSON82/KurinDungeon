#include "image.h"
#include "gameConfig.h"
#include "style.h"

const float animationFrameTime = 0.1f;
const float feetFromTop = 57.0f;

void DrawPart(Texture2D texture, Rectangle fromImage, Rectangle toScreen, Color tint)
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

static void DrawFrame(Texture2D texture, int frame, Vector2 feet, float scale, bool flip, Color tint)
{
    const float frameSize = spriteFrameSize;
    Rectangle fromImage{frame * frameSize, 0.0f, flip ? -frameSize : frameSize, frameSize};
    float size = frameSize * scale;
    Rectangle toScreen{feet.x - size / 2.0f, feet.y - feetFromTop * scale, size, size};
    DrawPart(texture, fromImage, toScreen, tint);
}

void DrawSprite(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color tint)
{
    DrawFrame(sheet.texture, frame, feet, worldScale, flip, tint);
}

void DrawPortrait(const SpriteSheet &sheet, int frame, Vector2 feet)
{
    DrawFrame(sheet.texture, frame, feet, portraitScale, false, WHITE);
}

void DrawSpriteOutline(const SpriteSheet &sheet, int frame, Vector2 feet, bool flip, Color color)
{
    float step = worldScale;
    DrawFrame(sheet.silhouette, frame, {feet.x - step, feet.y}, worldScale, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x + step, feet.y}, worldScale, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x, feet.y - step}, worldScale, flip, color);
    DrawFrame(sheet.silhouette, frame, {feet.x, feet.y + step}, worldScale, flip, color);
}

void DrawBox(const BoxImage &image, Rectangle box, Color tint)
{
    Rectangle part = image.part;
    float cornerWidth = image.cornerX * uiScale;
    float cornerHeight = image.cornerY * uiScale;

    float fromX[3] = {part.x, part.x + image.cornerX, part.x + part.width - image.cornerX};
    float fromWidth[3] = {image.cornerX, part.width - image.cornerX * 2, image.cornerX};
    float toX[3] = {box.x, box.x + cornerWidth, box.x + box.width - cornerWidth};
    float toWidth[3] = {cornerWidth, box.width - cornerWidth * 2, cornerWidth};

    float fromY[3] = {part.y, part.y + image.cornerY, part.y + part.height - image.cornerY};
    float fromHeight[3] = {image.cornerY, part.height - image.cornerY * 2, image.cornerY};
    float toY[3] = {box.y, box.y + cornerHeight, box.y + box.height - cornerHeight};
    float toHeight[3] = {cornerHeight, box.height - cornerHeight * 2, cornerHeight};

    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < 3; column++)
        {
            Rectangle fromImage{fromX[column], fromY[row], fromWidth[column], fromHeight[row]};
            Rectangle toScreen{toX[column], toY[row], toWidth[column], toHeight[row]};
            DrawPart(image.texture, fromImage, toScreen, tint);
        }
    }
}

int GetLoopFrame(const SpriteSheet &sheet, float time)
{
    if (sheet.frameCount <= 0)
    {
        return 0;
    }
    return static_cast<int>(time / animationFrameTime) % sheet.frameCount;
}
