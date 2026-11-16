#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "core/data/cards.h"
#include "ecs/registry.h"

const int startingHp = 80;
const int restHeal = 20;
const int rewardChoiceCount = 3;

struct RunState
{
    std::uint32_t seed = 0;
    std::mt19937 rng;

    ecs::Entity player;
    ecs::Entity currentRoom;

    std::vector<CardId> rewardChoices;
};

enum class RunCommandType
{
    StartRun,
    EnterRoom,
    FinishBattle,
    TakeReward,
    TakeRest,
};

struct RunCommand
{
    RunCommandType type = RunCommandType::StartRun;
    ecs::Entity room;
    int choice = 0;
    std::uint32_t seed = 0;
};

struct RunCommands
{
    std::vector<RunCommand> queue;
};

inline void PushRunCommand(ecs::Registry &registry, RunCommand command)
{
    registry.Resource<RunCommands>().queue.push_back(command);
}

enum class RunEventType
{
    RunStarted,
    BattleStarted,
    RestStarted,
    RewardOffered,
    RoomFinished,
    BattleLost,
    BossDefeated,
};

struct RunEvents
{
    std::vector<RunEventType> pending;
};
