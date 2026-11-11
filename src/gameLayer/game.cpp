#include <raylib.h>
#include <random>
#include "game.h"
#include "screen/battle/battleScreen.h"
#include "screen/characterSelect/characterSelectScreen.h"
#include "screen/logo/logoScreen.h"
#include "screen/lose/loseScreen.h"
#include "screen/map/mapScreen.h"
#include "screen/menu/menuScreen.h"
#include "screen/reward/rewardScreen.h"
#include "screen/win/winScreen.h"

// Change screen
void Game::ChangeScreen(GameScreen nextScreen)
{
    if (nextScreen == GameScreen::Logo)
    {
        logo = LogoScreenState{};
    }
    screen = nextScreen;
}

// New run
void Game::StartRun()
{
    // Reset run
    battle.reset();
    run = RunState{};

    // Seed
    run.seed = std::random_device{}();
    run.rng.seed(run.seed);
    TraceLog(LOG_INFO, "Run seed: %u", static_cast<unsigned>(run.seed));
}

// New battle
void Game::StartBattle(EnemyId enemyId)
{
    battle = std::make_unique<Battle>(run.player, enemyId, static_cast<std::uint32_t>(run.rng()));
}

// Battle result
void Game::UpdateBattle()
{
    if (!battle)
    {
        ChangeScreen(GameScreen::Map);
        return;
    }
    if (UpdateBattleScreen(*battle) != ScreenAction::LeaveBattle)
    {
        return;
    }

    BattleResult result = battle->GetResult();
    battle.reset();

    if (result == BattleResult::Won)
    {
        ChangeScreen(GameScreen::Reward);
    }
    else
    {
        ChangeScreen(GameScreen::Lose);
    }
}

// Update
void Game::Update(float deltaTime)
{
    switch (screen)
    {
    case GameScreen::Logo:
        if (UpdateLogoScreen(logo, deltaTime) == ScreenAction::LogoFinished)
        {
            ChangeScreen(GameScreen::Menu);
        }
        break;
    case GameScreen::Menu:
        if (UpdateMenuScreen() == ScreenAction::StartPressed)
        {
            ChangeScreen(GameScreen::CharacterSelect);
        }
        break;
    case GameScreen::CharacterSelect:
        if (UpdateCharacterSelectScreen() == ScreenAction::CharacterConfirmed)
        {
            StartRun();
            ChangeScreen(GameScreen::Map);
        }
        break;
    case GameScreen::Map:
        if (UpdateMapScreen() == ScreenAction::CombatChosen)
        {
            StartBattle(EnemyId::Slime);
            ChangeScreen(GameScreen::Battle);
        }
        break;
    case GameScreen::Battle:
        UpdateBattle();
        break;
    case GameScreen::Reward:
        if (UpdateRewardScreen() == ScreenAction::RewardConfirmed)
        {
            ChangeScreen(GameScreen::Map);
        }
        break;
    case GameScreen::Lose:
        if (UpdateLoseScreen() == ScreenAction::ReturnToMenu)
        {
            ChangeScreen(GameScreen::Menu);
        }
        break;
    case GameScreen::Win:
        if (UpdateWinScreen() == ScreenAction::ReturnToMenu)
        {
            ChangeScreen(GameScreen::Menu);
        }
        break;
    }
}

// Draw
void Game::Draw() const
{
    switch (screen)
    {
    case GameScreen::Logo:
        DrawLogoScreen();
        break;
    case GameScreen::Menu:
        DrawMenuScreen();
        break;
    case GameScreen::CharacterSelect:
        DrawCharacterSelectScreen();
        break;
    case GameScreen::Map:
        DrawMapScreen();
        break;
    case GameScreen::Battle:
        if (battle)
        {
            DrawBattleScreen(*battle);
        }
        break;
    case GameScreen::Reward:
        DrawRewardScreen();
        break;
    case GameScreen::Lose:
        DrawLoseScreen();
        break;
    case GameScreen::Win:
        DrawWinScreen();
        break;
    }
}
