#pragma once
#include "assets/assets.h"
#include "screen/screenAction.h"

struct LogoScreenState
{
    float elapsed = 0.0f;
};

ScreenAction UpdateLogoScreen(LogoScreenState &state, float deltaTime);

void DrawLogoScreen(const LogoScreenState &state, const Assets &assets);
