#include "ui/Image.h"
#include "engine/GameConfig.h"
#include "ui/Style.h"

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

static void DrawFrame(Texture2D texture, std::size_t frame, Vector2 feet, float scale, bool flip, Color tint)
{
    const float frameSize = spriteFrameSize;
    const float feetFromTop = 57.0f; // the row of the frame where the character stands

    Rectangle fromImage{static_cast<float>(frame) * frameSize, 0.0f, flip ? -frameSize : frameSize, frameSize};
    float size = frameSize * scale;
    Rectangle toScreen{feet.x - size / 2.0f, feet.y - feetFromTop * scale, size, size};
    DrawPart(texture, fromImage, toScreen, tint);
}

void DrawSprite(const Animation &animation, std::size_t frame, Vector2 feet, bool flip, Color tint)
{
    DrawFrame(animation.getTexture(), frame, feet, worldScale, flip, tint);
}

void DrawPortrait(const Animation &animation, std::size_t frame, Vector2 feet)
{
    DrawFrame(animation.getTexture(), frame, feet, portraitScale, false, WHITE);
}

void DrawSpriteOutline(const Animation &animation, std::size_t frame, Vector2 feet, bool flip, Color color)
{
    Texture2D silhouette = animation.getSilhouette();
    float step = worldScale; // one art pixel
    DrawFrame(silhouette, frame, {feet.x - step, feet.y}, worldScale, flip, color);
    DrawFrame(silhouette, frame, {feet.x + step, feet.y}, worldScale, flip, color);
    DrawFrame(silhouette, frame, {feet.x, feet.y - step}, worldScale, flip, color);
    DrawFrame(silhouette, frame, {feet.x, feet.y + step}, worldScale, flip, color);
}

// Nine-slice drawing. The box art is cut into 3 x 3 pieces:
//
//     corner | edge   | corner
//     edge   | middle | edge
//     corner | edge   | corner
//
// The corners keep their shape and the edges and middle stretch, so one small image can
// make a box of any size.
void DrawBox(const BoxImage &image, Rectangle box, Color tint)
{
    Rectangle part = image.part;
    float cornerWidth = image.cornerX * uiScale; // corner size on screen
    float cornerHeight = image.cornerY * uiScale;

    // Where each of the three columns starts and how wide it is, in the image and on screen.
    float fromX[3] = {part.x, part.x + image.cornerX, part.x + part.width - image.cornerX};
    float fromWidth[3] = {image.cornerX, part.width - image.cornerX * 2, image.cornerX};
    float toX[3] = {box.x, box.x + cornerWidth, box.x + box.width - cornerWidth};
    float toWidth[3] = {cornerWidth, box.width - cornerWidth * 2, cornerWidth};

    // The same for the three rows.
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
