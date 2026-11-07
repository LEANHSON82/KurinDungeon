#pragma once
#include <memory>
#include "combat/battle.h"
#include "run/runState.h"
#include "screen/screen.h"

class Game
{
public:
    void Update(float deltaTime);
    void Draw() const;

private:
    void ChangeScreen(GameScreen nextScreen);
    void StartRun();
    void StartBattle();
    void UpdateBattle();

    GameScreen screen = GameScreen::Logo;
    RunState run;
    std::unique_ptr<Battle> battle;
};
