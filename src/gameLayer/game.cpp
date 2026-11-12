#include <raylib.h>
#include <random>
#include "game.h"
#include "cards/cardDatabase.h"
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

    OnBattleOver(battle->GetResult());
}

// Enter room
void Game::EnterRoom(int roomId)
{
    if (!run.map.CanVisit(run.currentRoom, roomId))
    {
        return;
    }

    const Room &room = run.map.GetRoom(roomId);
    if (room.type != RoomType::Combat)
    {
        return;
    }

    run.currentRoom = roomId;
    StartBattle(room.enemy);
    ChangeScreen(GameScreen::Battle);
}

// Battle over
void Game::OnBattleOver(BattleResult result)
{
    battle.reset();

    if (result == BattleResult::Won)
    {
        run.map.MarkCleared(run.currentRoom);
        run.rewardChoices = RollRewards(run.rng, 3);
        ChangeScreen(GameScreen::Reward);
    }
    else
    {
        ChangeScreen(GameScreen::Lose);
    }
}

// Take reward
void Game::TakeReward(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(run.rewardChoices.size()))
    {
        return;
    }

    run.player.masterDeck.push_back(run.rewardChoices[choiceIndex]);
    run.rewardChoices.clear();

    if (run.map.GetRoom(run.currentRoom).next.empty())
    {
        ChangeScreen(GameScreen::Win);
    }
    else
    {
        ChangeScreen(GameScreen::Map);
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
    {
        int chosenRoom = -1;
        if (UpdateMapScreen(run.map, run.currentRoom, chosenRoom) == ScreenAction::RoomChosen)
        {
            EnterRoom(chosenRoom);
        }
        break;
    }
    case GameScreen::Battle:
        UpdateBattle();
        break;
    case GameScreen::Reward:
    {
        int chosenIndex = -1;
        if (UpdateRewardScreen(run.rewardChoices, chosenIndex) == ScreenAction::CardChosen)
        {
            TakeReward(chosenIndex);
        }
        break;
    }
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
        DrawMapScreen(run.map, run.currentRoom);
        break;
    case GameScreen::Battle:
        if (battle)
        {
            DrawBattleScreen(*battle);
        }
        break;
    case GameScreen::Reward:
        DrawRewardScreen(run.rewardChoices);
        break;
    case GameScreen::Lose:
        DrawLoseScreen();
        break;
    case GameScreen::Win:
        DrawWinScreen();
        break;
    }
}
