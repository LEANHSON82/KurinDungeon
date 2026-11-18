#pragma once
#include <deque>
#include <random>
#include <vector>
#include "ecs/registry.h"

inline constexpr int startingEnergy = 3;
inline constexpr int handSize = 5;

enum class BattleResult
{
    InProgress,
    Won,
    Lost,
};

struct BattleState
{
    bool active = false;
    int energy = 0;
    int turn = 1;
    BattleResult result = BattleResult::InProgress;
    std::mt19937 rng;
    int nextOrder = 0;
};

enum class BattleCommandType
{
    PlayCard,
    EndTurn,
};

struct BattleCommand
{
    BattleCommandType type = BattleCommandType::PlayCard;
    ecs::Entity card;
    ecs::Entity target;
};

struct BattleCommands
{
    std::vector<BattleCommand> queue;
};

inline BattleCommand PlayCardCommand(ecs::Entity card, ecs::Entity target)
{
    return {BattleCommandType::PlayCard, card, target};
}

inline BattleCommand EndTurnCommand()
{
    return {BattleCommandType::EndTurn, ecs::nullEntity, ecs::nullEntity};
}

inline void PushBattleCommand(ecs::Registry &registry, BattleCommand command)
{
    registry.Resource<BattleCommands>().queue.push_back(command);
}

enum class BattleEventType
{
    Damage,
    Block,
    Buff,
    Death,
};

struct BattleEvent
{
    BattleEventType type = BattleEventType::Damage;
    ecs::Entity source;
    ecs::Entity target;
    int value = 0;
};

struct BattleEvents
{
    std::deque<BattleEvent> pending;
};
