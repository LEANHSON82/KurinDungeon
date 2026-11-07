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
        ResetLogoScreen();
    }
    screen = nextScreen;
}

void Game::StartRun()
{
    battle.reset();
    run = RunState{};
}

void Game::StartBattle()
{
    battle = std::make_unique<Battle>(run.player, EnemyId::Slime);
}

void Game::UpdateBattle()
{
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
        if (UpdateLogoScreen(deltaTime) == ScreenAction::LogoFinished)
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
            StartBattle();
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
        DrawBattleScreen(*battle);
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
