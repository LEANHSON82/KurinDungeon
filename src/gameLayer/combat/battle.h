#pragma once
#include <cstdint>
#include "cards/deck.h"
#include "combat/enemies/enemy.h"
#include "run/player.h"

// Result
enum class BattleResult
{
    InProgress,
    Won,
    Lost,
};

class Battle
{
public:
    // Rules
    static constexpr int startingEnergy = 3;
    static constexpr int handSize = 5;

    Battle(PlayerState &playerState, EnemyId enemyId, std::uint32_t seed);

    // Actions
    bool CanPlayCard(int handIndex) const;
    bool PlayCard(int handIndex);
    void EndPlayerTurn();

    // Read only
    const PlayerState &GetPlayer() const { return player; }
    const Enemy &GetEnemy() const { return enemy; }
    const Deck &GetDeck() const { return deck; }
    int GetEnergy() const { return energy; }
    int GetPlayerBlock() const { return playerBlock; }
    int GetTurn() const { return turn; }
    BattleResult GetResult() const { return result; }

private:
    // Turn flow
    void StartPlayerTurn();
    void ExecuteEnemyTurn();
    void CheckResult();

    // Borrowed player
    PlayerState &player;

    // Battle state
    Enemy enemy;
    Deck deck;
    int energy = 0;
    int playerBlock = 0;
    int turn = 1;
    BattleResult result = BattleResult::InProgress;
};