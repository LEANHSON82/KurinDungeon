#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "cards/card.h"
#include "map/dungeonMap.h"
#include "relics/relic.h"
#include "player.h"

const int startingGold = 99;
const int minCombatGold = 10;
const int maxCombatGold = 20;

struct RunState
{
    PlayerState player;
    int gold = startingGold;
    std::vector<RelicId> relics;

    DungeonMap map;
    int currentRoom = 0;

    std::vector<CardId> rewardChoices;
    int rewardGold = 0;

    std::uint32_t seed = 0;
    std::mt19937 rng;
};
