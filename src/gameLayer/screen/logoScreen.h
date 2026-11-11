#pragma once
#include "screenAction.h"

struct LogoScreenState
{
    float elapsed = 0.0f;
};

ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime);
void DrawLogoScreen();
