#pragma once
#include <cstdint>
#include <vector>
#include "cards/deck.h"
#include "combat/battleEvent.h"
#include "combat/enemies/enemy.h"
#include "run/player.h"

enum class BattleResult
{
    InProgress,
    Won,
    Lost,
};

class Battle
{
public:
    static constexpr int startingEnergy = 3;
    static constexpr int handSize = 5;

    Battle(PlayerState &playerState, const std::vector<EnemyId> &enemyIds, std::uint32_t seed);

    bool CanPlayCard(int handIndex, int targetIndex) const;
    bool PlayCard(int handIndex, int targetIndex);
    void EndPlayerTurn();

    const PlayerState &GetPlayer() const { return player; }
    const std::vector<Enemy> &GetEnemies() const { return enemies; }
    bool IsEnemyAlive(int index) const;
    const Deck &GetDeck() const { return deck; }
    int GetEnergy() const { return energy; }
    int GetPlayerBlock() const { return playerBlock; }
    int GetTurn() const { return turn; }
    BattleResult GetResult() const { return result; }

    std::vector<BattleEvent> TakeEvents();

private:
    void StartPlayerTurn();
    void ExecuteEnemyTurn();
    void CheckResult();
    void ChooseEnemyIntents();
    std::vector<int> GetTargets(const Card &card, int targetIndex) const;
    void AddEvent(BattleEventType type, int source, int target, int value);

    PlayerState &player;

    std::vector<Enemy> enemies;
    Deck deck;
    int energy = 0;
    int playerBlock = 0;
    int turn = 1;
    BattleResult result = BattleResult::InProgress;
    std::vector<BattleEvent> events;
};