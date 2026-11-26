#pragma once
#include <cstdint>
#include <vector>
#include "cards/deck.h"
#include "combat/battleEvent.h"
#include "combat/enemies/enemy.h"
#include "relics/relic.h"
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

    Battle(PlayerState &playerState, const std::vector<EnemyId> &enemyIds, const std::vector<RelicId> &relicIds, std::uint32_t seed);

    bool CanPlayCard(int handIndex, int targetIndex) const;
    bool PlayCard(int handIndex, int targetIndex);
    void EndPlayerTurn();

    int ModifyDamage(int source, int target, int base) const;
    const Statuses &GetStatuses(int actor) const;

    const PlayerState &GetPlayer() const { return player; }
    const std::vector<Enemy> &GetEnemies() const { return enemies; }
    const std::vector<RelicId> &GetRelics() const { return relics; }
    bool IsEnemyAlive(int index) const;
    const Deck &GetDeck() const { return deck; }
    int GetEnergy() const { return energy; }
    int GetPlayerBlock() const { return playerBlock; }
    int GetTurn() const { return turn; }
    BattleResult GetResult() const { return result; }

    std::vector<BattleEvent> TakeEvents();

private:
    void StartPlayerTurn();
    void ChooseEnemyIntents();
    void TriggerRelics(RelicTrigger trigger);
    void ExecuteEnemyTurn();
    void PerformIntent(int enemyIndex);
    void TickStatuses(Statuses &statuses, const Statuses &hadAtStart);
    std::vector<int> GetTargets(const Card &card, int targetIndex) const;
    void ApplyEffectToPlayer(const Effect &effect, Target target);
    void ApplyEffectToEnemy(const Effect &effect, int enemyIndex);
    void DealDamage(int source, int target, int amount);
    void GainBlock(int source, int target, int amount);
    void LoseHp(int target, int amount);
    void ApplyStatus(int source, int target, StatusType type, int amount);
    int &EditHp(int actor);
    int &EditBlock(int actor);
    Statuses &EditStatuses(int actor);
    void CheckResult();
    void AddEvent(BattleEventType type, int source, int target, int value, StatusType status = StatusType::Strength);

    PlayerState &player;
    Statuses playerStatus;

    std::vector<Enemy> enemies;
    std::vector<RelicId> relics;
    Deck deck;
    int energy = 0;
    int playerBlock = 0;
    int turn = 1;
    BattleResult result = BattleResult::InProgress;
    std::vector<BattleEvent> events;
};
