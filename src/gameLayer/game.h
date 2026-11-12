#pragma once
#include <memory>
#include "combat/battle.h"
#include "run/runState.h"
#include "screen/logo/logoScreen.h"
#include "screen/screen.h"

class Game
{
public:
    // Frame
    void Update(float deltaTime);
    void Draw() const;

private:
    // Flow
    void ChangeScreen(GameScreen nextScreen);
    void StartRun();
    void StartBattle(EnemyId enemyId);
    void UpdateBattle();
    void EnterRoom(int roomId);
    void OnBattleOver(BattleResult result);
    void TakeReward(int choiceIndex);

    // State
    GameScreen screen = GameScreen::Logo;
    LogoScreenState logo;
    RunState run;
    std::unique_ptr<Battle> battle;
};
