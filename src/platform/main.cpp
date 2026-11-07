#include <raylib.h>
#include "gameConfig.h"
#include "game.h"

int main()
{
    InitWindow(screenWidth, screenHeight, "Kurin");
    SetTargetFPS(60);

    {
        Game game;
        while (!WindowShouldClose())
        {
            game.Update(GetFrameTime());

            BeginDrawing();
            ClearBackground(RAYWHITE);
            game.Draw();
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
