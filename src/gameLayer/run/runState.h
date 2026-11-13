#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "cards/card.h"
#include "map/dungeonMap.h"
#include "player.h"

struct RunState
{
    // Player
    PlayerState player;

    // Map
    DungeonMap map;
    int currentRoom = 0;

    // Rewards
    std::vector<CardId> rewardChoices;

    // Randomness
    std::uint32_t seed = 0;
    std::mt19937 rng;
};
