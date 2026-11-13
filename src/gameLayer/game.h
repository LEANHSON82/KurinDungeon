#pragma once
#include <memory>
#include "assets/assets.h"
#include "combat/battle.h"
#include "run/runState.h"
#include "screen/battle/battleScreen.h"
#include "screen/logo/logoScreen.h"
#include "screen/screen.h"

class Game
{
public:
    Game();
    ~Game();
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;

    void Update(float deltaTime);
    void Draw() const;
    bool ShouldQuit() const { return quit; }

private:
    void ChangeScreen(GameScreen nextScreen);
    void StartRun();
    void StartBattle(const std::vector<EnemyId> &enemyIds);
    void UpdateBattle(float deltaTime);
    void EnterRoom(int roomId);
    void OnBattleOver(BattleResult result);
    void TakeReward(int choiceIndex);
    void TakeRest();
    void FinishRoom();

    Assets assets;

    GameScreen screen = GameScreen::Logo;
    bool quit = false;
    LogoScreenState logo;
    RunState run;
    std::unique_ptr<Battle> battle;
    BattleScreenState battleView;
};
