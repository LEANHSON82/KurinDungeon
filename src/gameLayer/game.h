#pragma once
#include <memory>
#include "combat/battle.h"
#include "run/runState.h"
#include "screen/logoScreen.h"
#include "screen/screen.h"

class Game
{
public:
    void Update(float deltaTime);
    void Draw() const;

private:
    void ChangeScreen(GameScreen nextScreen);
    void StartRun();
    void StartBattle(EnemyId enemyId);
    void UpdateBattle();

    GameScreen screen = GameScreen::Logo;
    LogoScreenState logo;
    RunState run;
    std::unique_ptr<Battle> battle;
};
