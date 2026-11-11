#include <raylib.h>
#include <random>
#include "game.h"
#include "screen/battleScreen.h"
#include "screen/characterSelectScreen.h"
#include "screen/logoScreen.h"
#include "screen/loseScreen.h"
#include "screen/mapScreen.h"
#include "screen/menuScreen.h"
#include "screen/rewardScreen.h"
#include "screen/winScreen.h"

void Game::ChangeScreen(GameScreen nextScreen)
{
    if (nextScreen == GameScreen::Logo)
    {
        logo = LogoScreenState{};
    }
    screen = nextScreen;
}

void Game::StartRun()
{
    battle.reset();
    run = RunState{};
    run.seed = std::random_device{}();
    run.rng.seed(run.seed);
    TraceLog(LOG_INFO, "Run seed: %u", static_cast<unsigned>(run.seed));
}

void Game::StartBattle(EnemyId enemyId)
{
    battle = std::make_unique<Battle>(run.player, enemyId, static_cast<std::uint32_t>(run.rng()));
}

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
