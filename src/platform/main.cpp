#include <algorithm>
#include <raylib.h>
#include "game/game.h"
#include "game/gameConfig.h"

// Longest frame step the game simulates; longer pauses (loading, window drags) are clamped.
static constexpr float maxFrameTime = 0.05f;

static Rectangle GetViewport()
{
    float scale = std::min(GetScreenWidth() / static_cast<float>(screenWidth),
                           GetScreenHeight() / static_cast<float>(screenHeight));
    float width = screenWidth * scale;
    float height = screenHeight * scale;
    return {(GetScreenWidth() - width) / 2.0f, (GetScreenHeight() - height) / 2.0f, width, height};
}

int main()
{
    // Frame rate is capped by SetTargetFPS only; adding VSync on top judders on high refresh screens.
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Kurin");
    MaximizeWindow();
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    RenderTexture2D canvas = LoadRenderTexture(screenWidth, screenHeight);
    int canvasFilter = -1;
    {
        Game game;
        while (!WindowShouldClose() && !game.ShouldQuit())
        {
            Rectangle viewport = GetViewport();
            float scale = viewport.width / screenWidth;
            // Nearest keeps pixel art crisp when enlarging; bilinear avoids shimmer when shrinking.
            int filter = scale >= 1.0f ? TEXTURE_FILTER_POINT : TEXTURE_FILTER_BILINEAR;
            if (filter != canvasFilter)
            {
                SetTextureFilter(canvas.texture, filter);
                canvasFilter = filter;
            }
            SetMouseOffset(static_cast<int>(-viewport.x), static_cast<int>(-viewport.y));
            SetMouseScale(1.0f / scale, 1.0f / scale);

            game.Update(std::min(GetFrameTime(), maxFrameTime));

            BeginTextureMode(canvas);
            ClearBackground(BLACK);
            game.Draw();
            EndTextureMode();

            BeginDrawing();
            ClearBackground(BLACK);
            Rectangle source{0.0f, 0.0f, static_cast<float>(screenWidth), -static_cast<float>(screenHeight)};
            DrawTexturePro(canvas.texture, source, viewport, {0.0f, 0.0f}, 0.0f, WHITE);
            EndDrawing();
        }
    }
    UnloadRenderTexture(canvas);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
