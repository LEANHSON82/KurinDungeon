#pragma once
#include "screen/screenAction.h"

// Timer
struct LogoScreenState
{
    float elapsed = 0.0f;
};

// Update
ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime);

// Draw
void DrawLogoScreen();
