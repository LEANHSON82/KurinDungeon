#include <algorithm>
#include <random>
#include <raylib.h>
#include "game.h"
#include "cards/cardDatabase.h"
#include "relics/relicDatabase.h"
#include "screen/characterSelect/characterSelectScreen.h"
#include "screen/end/endScreen.h"
#include "screen/map/mapScreen.h"
#include "screen/menu/menuScreen.h"
#include "screen/rest/restScreen.h"
#include "screen/reward/rewardScreen.h"

Game::Game()
{
    LoadAssets(assets);
}

Game::~Game()
{
    battle.reset();
    UnloadAssets(assets);
}

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

    run.player.masterDeck = GetStartingDeck();
    run.relics = {RelicId::FieldBandage};
    run.map = DungeonMap::Generate(run.seed);
    run.currentRoom = run.map.GetStartRoom();
}

void Game::StartBattle(const std::vector<EnemyId> &enemyIds)
{
    battle = std::make_unique<Battle>(run.player, enemyIds, run.relics, static_cast<std::uint32_t>(run.rng()));
    battleView = BattleScreenState{};
    battleView.enemies.resize(enemyIds.size());
}

void Game::UpdateBattle(float deltaTime)
{
    if (!battle)
    {
        ChangeScreen(GameScreen::Map);
        return;
    }
    if (UpdateBattleScreen(battleView, *battle, assets, deltaTime) != ScreenAction::LeaveBattle)
    {
        return;
    }

    OnBattleOver(battle->GetResult());
}

void Game::EnterRoom(int roomId)
{
    if (!run.map.CanVisit(run.currentRoom, roomId))
    {
        return;
    }

    const Room &room = run.map.GetRoom(roomId);
    switch (room.type)
    {
    case RoomType::Combat:
    case RoomType::Boss:
        run.currentRoom = roomId;
        StartBattle(room.enemies);
        ChangeScreen(GameScreen::Battle);
        break;
    case RoomType::Rest:
        run.currentRoom = roomId;
        ChangeScreen(GameScreen::Rest);
        break;
    case RoomType::Start:
        break;
    }
}

void Game::OnBattleOver(BattleResult result)
{
    battle.reset();

    if (result == BattleResult::Lost)
    {
        ChangeScreen(GameScreen::GameOver);
        return;
    }

    if (run.map.GetRoom(run.currentRoom).type == RoomType::Boss)
    {
        run.map.MarkCleared(run.currentRoom);
        ChangeScreen(GameScreen::Victory);
        return;
    }

    ApplyBattleWonRelics();
    std::uniform_int_distribution<int> rollGold(minCombatGold, maxCombatGold);
    run.rewardGold = rollGold(run.rng);
    run.gold += run.rewardGold;
    run.rewardChoices = RollRewards(run.rng, rewardChoiceCount);
    ChangeScreen(GameScreen::Reward);
}

void Game::ApplyBattleWonRelics()
{
    for (RelicId id : run.relics)
    {
        const Relic &relic = GetRelic(id);
        if (relic.trigger != RelicTrigger::BattleWon)
        {
            continue;
        }

        if (relic.effect.type == EffectType::Heal)
        {
            run.player.hp = std::min(run.player.hp + relic.effect.value, run.player.maxHp);
        }
        else if (relic.effect.type == EffectType::GainGold)
        {
            run.gold += relic.effect.value;
        }
    }
}

void Game::TakeReward(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(run.rewardChoices.size()))
    {
        return;
    }

    run.player.masterDeck.push_back(run.rewardChoices[choiceIndex]);
    run.rewardChoices.clear();
    FinishRoom();
}

void Game::TakeRest()
{
    run.player.hp = std::min(run.player.hp + restHeal, run.player.maxHp);
    FinishRoom();
}

void Game::FinishRoom()
{
    run.map.MarkCleared(run.currentRoom);
    ChangeScreen(GameScreen::Map);
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
    {
        ScreenAction action = UpdateMenuScreen();
        if (action == ScreenAction::StartPressed)
        {
            ChangeScreen(GameScreen::CharacterSelect);
        }
        else if (action == ScreenAction::QuitPressed)
        {
            quit = true;
        }
        break;
    }
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
        UpdateBattle(deltaTime);
        break;
    case GameScreen::Reward:
    {
        int chosenIndex = -1;
        ScreenAction action = UpdateRewardScreen(run.rewardChoices, chosenIndex);
        if (action == ScreenAction::CardChosen)
        {
            TakeReward(chosenIndex);
        }
        else if (action == ScreenAction::RewardSkipped)
        {
            run.rewardChoices.clear();
            FinishRoom();
        }
        break;
    }
    case GameScreen::Rest:
        if (UpdateRestScreen() == ScreenAction::RestTaken)
        {
            TakeRest();
        }
        break;
    case GameScreen::Victory:
    case GameScreen::GameOver:
        if (UpdateEndScreen() == ScreenAction::ReturnToMenu)
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
        DrawLogoScreen(logo, assets);
        break;
    case GameScreen::Menu:
        DrawMenuScreen(assets);
        break;
    case GameScreen::CharacterSelect:
        DrawCharacterSelectScreen(assets);
        break;
    case GameScreen::Map:
        DrawMapScreen(run, assets);
        break;
    case GameScreen::Battle:
        if (battle)
        {
            DrawBattleScreen(battleView, *battle, assets);
        }
        break;
    case GameScreen::Reward:
        DrawRewardScreen(run.rewardChoices, run.rewardGold, assets);
        break;
    case GameScreen::Rest:
        DrawRestScreen(run.player, assets);
        break;
    case GameScreen::Victory:
        DrawEndScreen(true, assets);
        break;
    case GameScreen::GameOver:
        DrawEndScreen(false, assets);
        break;
    }
}
