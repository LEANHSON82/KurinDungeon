#pragma once

enum class GameScreen
{
    logo,
    menu,
    characterSelect,
    map,
    battle,
    reward,
    event,
    lose,
    win,
};

void UpdateScreen(float deltaTime);
void DrawScreen();
