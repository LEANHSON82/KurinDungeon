#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "cards/card.h"
#include "map/dungeonMap.h"
#include "player.h"

struct RunState
{
    PlayerState player;

    DungeonMap map;
    int currentRoom = 0;

    std::vector<CardId> rewardChoices;

    std::uint32_t seed = 0;
    std::mt19937 rng;
};
