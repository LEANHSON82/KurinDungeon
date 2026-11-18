#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "core/data/cards.h"
#include "ecs/registry.h"

inline constexpr int startingHp = 80;
inline constexpr int restHeal = 20;
inline constexpr int rewardChoiceCount = 3;

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

inline RunCommand StartRunCommand(std::uint32_t seed)
{
    RunCommand command;
    command.type = RunCommandType::StartRun;
    command.seed = seed;
    return command;
}

inline RunCommand EnterRoomCommand(ecs::Entity room)
{
    RunCommand command;
    command.type = RunCommandType::EnterRoom;
    command.room = room;
    return command;
}

inline RunCommand FinishBattleCommand()
{
    RunCommand command;
    command.type = RunCommandType::FinishBattle;
    return command;
}

inline RunCommand TakeRewardCommand(int choice)
{
    RunCommand command;
    command.type = RunCommandType::TakeReward;
    command.choice = choice;
    return command;
}

inline RunCommand TakeRestCommand()
{
    RunCommand command;
    command.type = RunCommandType::TakeRest;
    return command;
}

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
