#pragma once
#include <memory>
#include "combat/battle.h"
#include "run/runState.h"
#include "screen/battle/battleScreen.h"
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
    void StartBattle(const std::vector<EnemyId> &enemyIds);
    void UpdateBattle();
    void EnterRoom(int roomId);
    void OnBattleOver(BattleResult result);
    void TakeReward(int choiceIndex);
    void TakeRest();
    void FinishRoom();

    // State
    GameScreen screen = GameScreen::Logo;
    LogoScreenState logo;
    RunState run;
    std::unique_ptr<Battle> battle;
    BattleScreenState battleView;
};
