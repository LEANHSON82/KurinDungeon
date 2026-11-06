#include "screen.h"
#include "logoScreen.h"
#include "menuScreen.h"
#include "characterSelectScreen.h"
#include "mapScreen.h"
#include "battleScreen.h"
#include "rewardScreen.h"
#include "loseScreen.h"
#include "winScreen.h"
#include "run/runState.h"

static GameScreen currentScreen = GameScreen::logo;
static RunState runState;
static void ChangeScreen(GameScreen nextScreen)
{
    if (currentScreen == nextScreen)
    {
        return;
    }
    if (nextScreen == GameScreen::logo)
    {
        ResetLogoScreen();
    }
    currentScreen = nextScreen;
}

void UpdateScreen(float deltaTime)
{
    switch (currentScreen)
    {
    case GameScreen::logo:
    {
        ScreenAction action = UpdateLogoScreen(deltaTime);
        if (action == ScreenAction::logoFinished)
        {
            ChangeScreen(GameScreen::menu);
        }
        break;
    }
    case GameScreen::menu:
    {
        ScreenAction action = UpdateMenuScreen();
        if (action == ScreenAction::startPressed)
        {
            ChangeScreen(GameScreen::characterSelect);
        }
        break;
    }
    case GameScreen::characterSelect:
    {
        ScreenAction action = UpdateCharacterSelectScreen();
        if (action == ScreenAction::characterConfirmed)
        {
            runState = RunState{};
            ChangeScreen(GameScreen::map);
        }
        break;
    }
    case GameScreen::map:
    {
        ScreenAction action = UpdateMapScreen();
        if (action == ScreenAction::combatChosen)
        {
            ChangeScreen(GameScreen::battle);
        }
        break;
    }
    case GameScreen::battle:
    {
        ScreenAction action = UpdateBattleScreen();
        if (action == ScreenAction::returnToMap)
        {
            ChangeScreen(GameScreen::map);
        }
        break;
    }

    case GameScreen::reward:
    {
        ScreenAction action = UpdateRewardScreen();

        if (action == ScreenAction::rewardConfirmed)
        {
            ChangeScreen(GameScreen::map);
        }
        break;
    }
    case GameScreen::lose:
    {
        ScreenAction action = UpdateLoseScreen();

        if (action == ScreenAction::returnToMenu)
        {
            ChangeScreen(GameScreen::menu);
        }
        break;
    }

    case GameScreen::win:
    {
        ScreenAction action = UpdateWinScreen();

        if (action == ScreenAction::returnToMenu)
        {
            ChangeScreen(GameScreen::menu);
        }
        break;
    }
    default:
        break;
    }
}

void DrawScreen()
{
    switch (currentScreen)
    {
    case GameScreen::logo:
        DrawLogoScreen();
        break;
    case GameScreen::menu:
        DrawMenuScreen();
        break;
    case GameScreen::characterSelect:
        DrawCharacterSelectScreen();
        break;
    case GameScreen::map:
        DrawMapScreen();
        break;
    case GameScreen::battle:
        DrawBattleScreen(runState.player);
        break;
    case GameScreen::reward:
        DrawRewardScreen();
        break;
    case GameScreen::lose:
        DrawLoseScreen();
        break;
    case GameScreen::win:
        DrawWinScreen();
        break;
    default:
        break;
    }
}
