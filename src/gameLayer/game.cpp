#include <algorithm>
#include <random>
#include <raylib.h>
#include "game.h"
#include "cards/cardDatabase.h"
#include "characters/characterDatabase.h"
#include "events/eventDatabase.h"
#include "relics/relicDatabase.h"
#include "save/saveGame.h"
#include "screen/characterSelect/characterSelectScreen.h"
#include "screen/end/endScreen.h"
#include "screen/event/eventScreen.h"
#include "screen/map/mapScreen.h"
#include "screen/menu/menuScreen.h"
#include "screen/rest/restScreen.h"
#include "screen/reward/rewardScreen.h"
#include "screen/shop/shopScreen.h"
#include "screen/treasure/treasureScreen.h"

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
    if (nextScreen == GameScreen::Menu)
    {
        hasSave = HasSave();
    }
    if (nextScreen == GameScreen::CharacterSelect)
    {
        characterSelect = CharacterSelectState{};
    }
    screen = nextScreen;
}

void Game::StartRun(CharacterId character)
{
    battle.reset();
    run = RunState{};

    run.seed = std::random_device{}();
    run.rng.seed(run.seed);
    TraceLog(LOG_INFO, "Run seed: %u", static_cast<unsigned>(run.seed));

    const CharacterDefinition &definition = GetCharacter(character);
    run.player.character = character;
    run.player.maxHp = definition.maxHp;
    run.player.hp = definition.maxHp;
    run.player.masterDeck = definition.startingDeck;
    run.relics = {definition.starterRelic};
    run.map = DungeonMap::Generate(run.seed);
    run.currentRoom = run.map.GetStartRoom();
    SaveRun(run);
}

void Game::ContinueRun()
{
    battle.reset();
    if (LoadRun(run))
    {
        ChangeScreen(GameScreen::Map);
        return;
    }
    DeleteSave();
    hasSave = false;
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
    run.currentRoom = roomId;
    switch (room.type)
    {
    case RoomType::Combat:
    case RoomType::Elite:
    case RoomType::Boss:
        StartBattle(room.enemies);
        ChangeScreen(GameScreen::Battle);
        break;
    case RoomType::Rest:
        ChangeScreen(GameScreen::Rest);
        break;
    case RoomType::Shop:
        EnterShop();
        ChangeScreen(GameScreen::Shop);
        break;
    case RoomType::Event:
        run.event = RollEvent(run.rng);
        ChangeScreen(GameScreen::Event);
        break;
    case RoomType::Treasure:
        run.hasRewardRelic = RollRelic(run.rng, run.relics, run.rewardRelic);
        ChangeScreen(GameScreen::Treasure);
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
        DeleteSave();
        ChangeScreen(GameScreen::GameOver);
        return;
    }

    if (run.map.GetRoom(run.currentRoom).type == RoomType::Boss)
    {
        run.map.MarkCleared(run.currentRoom);
        DeleteSave();
        ChangeScreen(GameScreen::Victory);
        return;
    }

    ApplyBattleWonRelics();
    bool elite = run.map.GetRoom(run.currentRoom).type == RoomType::Elite;
    std::uniform_int_distribution<int> rollGold(elite ? minEliteGold : minCombatGold, elite ? maxEliteGold : maxCombatGold);
    run.rewardGold = rollGold(run.rng);
    run.gold += run.rewardGold;
    run.hasRewardRelic = elite && RollRelic(run.rng, run.relics, run.rewardRelic);
    if (run.hasRewardRelic)
    {
        run.relics.push_back(run.rewardRelic);
    }
    run.rewardChoices = RollRewards(run.rng, rewardChoiceCount);
    ChangeScreen(GameScreen::Reward);
}

void Game::GainRandomRelic()
{
    RelicId relic = RelicId::OakShield;
    if (RollRelic(run.rng, run.relics, relic))
    {
        run.relics.push_back(relic);
        return;
    }
    run.gold += noRelicGold;
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

void Game::EnterShop()
{
    run.shop = ShopStock{};
    run.shop.cards = RollRewards(run.rng, shopCardCount);
    run.shop.cardSold.assign(run.shop.cards.size(), false);
    run.shop.hasRelic = RollRelic(run.rng, run.relics, run.shop.relic);
}

void Game::BuyShopItem(int item)
{
    ShopStock &shop = run.shop;
    int cardCountInShop = static_cast<int>(shop.cards.size());
    if (item >= 0 && item < cardCountInShop && !shop.cardSold[item] && run.gold >= GetCardPrice(shop.cards[item]))
    {
        run.gold -= GetCardPrice(shop.cards[item]);
        run.player.masterDeck.push_back(shop.cards[item]);
        shop.cardSold[item] = true;
        return;
    }
    if (item == cardCountInShop && shop.hasRelic && !shop.relicSold && run.gold >= relicPrice)
    {
        run.gold -= relicPrice;
        run.relics.push_back(shop.relic);
        shop.relicSold = true;
    }
}

void Game::ChooseEventOption(int choiceIndex)
{
    const Event &event = GetEvent(run.event);
    if (choiceIndex < 0 || choiceIndex >= static_cast<int>(event.choices.size()))
    {
        return;
    }

    for (const Outcome &outcome : event.choices[choiceIndex].outcomes)
    {
        switch (outcome.type)
        {
        case OutcomeType::LoseHp:
            run.player.hp = std::max(1, run.player.hp - outcome.value);
            break;
        case OutcomeType::Heal:
            run.player.hp = std::min(run.player.hp + outcome.value, run.player.maxHp);
            break;
        case OutcomeType::GainGold:
            run.gold += outcome.value;
            break;
        case OutcomeType::GainRelic:
            GainRandomRelic();
            break;
        case OutcomeType::GainCard:
            for (CardId card : RollRewards(run.rng, 1))
            {
                run.player.masterDeck.push_back(card);
            }
            break;
        }
    }
    FinishRoom();
}

void Game::TakeTreasure()
{
    if (run.hasRewardRelic)
    {
        run.relics.push_back(run.rewardRelic);
    }
    else
    {
        run.gold += noRelicGold;
    }
    run.hasRewardRelic = false;
    FinishRoom();
}

void Game::FinishRoom()
{
    run.map.MarkCleared(run.currentRoom);
    SaveRun(run);
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
        ScreenAction action = UpdateMenuScreen(hasSave);
        if (action == ScreenAction::StartPressed)
        {
            ChangeScreen(GameScreen::CharacterSelect);
        }
        else if (action == ScreenAction::ContinuePressed)
        {
            ContinueRun();
        }
        else if (action == ScreenAction::QuitPressed)
        {
            quit = true;
        }
        break;
    }
    case GameScreen::CharacterSelect:
        if (UpdateCharacterSelectScreen(characterSelect) == ScreenAction::CharacterConfirmed)
        {
            StartRun(static_cast<CharacterId>(characterSelect.selected));
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
    case GameScreen::Shop:
    {
        int chosenItem = -1;
        ScreenAction action = UpdateShopScreen(run.shop, run.gold, chosenItem);
        if (action == ScreenAction::ItemBought)
        {
            BuyShopItem(chosenItem);
        }
        else if (action == ScreenAction::ShopLeft)
        {
            FinishRoom();
        }
        break;
    }
    case GameScreen::Event:
    {
        int chosenIndex = -1;
        if (UpdateEventScreen(run.event, chosenIndex) == ScreenAction::EventChoiceMade)
        {
            ChooseEventOption(chosenIndex);
        }
        break;
    }
    case GameScreen::Treasure:
        if (UpdateTreasureScreen() == ScreenAction::TreasureTaken)
        {
            TakeTreasure();
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
        DrawMenuScreen(hasSave, assets);
        break;
    case GameScreen::CharacterSelect:
        DrawCharacterSelectScreen(characterSelect, assets);
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
        DrawRewardScreen(run.rewardChoices, run.rewardGold, run.hasRewardRelic, run.rewardRelic, assets);
        break;
    case GameScreen::Rest:
        DrawRestScreen(run.player, assets);
        break;
    case GameScreen::Shop:
        DrawShopScreen(run.shop, run.gold, assets);
        break;
    case GameScreen::Event:
        DrawEventScreen(run.event, assets);
        break;
    case GameScreen::Treasure:
        DrawTreasureScreen(run.hasRewardRelic, run.rewardRelic, assets);
        break;
    case GameScreen::Victory:
        DrawEndScreen(true, assets);
        break;
    case GameScreen::GameOver:
        DrawEndScreen(false, assets);
        break;
    }
}
