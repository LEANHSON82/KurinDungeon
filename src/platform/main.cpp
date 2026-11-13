#include <algorithm>
#include <raylib.h>
#include "gameConfig.h"
#include "game.h"

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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Kurin");
    MaximizeWindow();
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    RenderTexture2D canvas = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);
    {
        Game game;
        while (!WindowShouldClose() && !game.ShouldQuit())
        {
            Rectangle viewport = GetViewport();
            float scale = viewport.width / screenWidth;
            SetMouseOffset(static_cast<int>(-viewport.x), static_cast<int>(-viewport.y));
            SetMouseScale(1.0f / scale, 1.0f / scale);

            game.Update(GetFrameTime());

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
