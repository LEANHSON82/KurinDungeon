#include <raylib.h>
#include "gameConfig.h"
#include "screen/screen.h"

int main()
{
    InitWindow(screenWidth, screenHeight, "Kurin");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        UpdateScreen(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawScreen();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
