#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "cards/card.h"
#include "map/dungeonMap.h"
#include "events/event.h"
#include "relics/relic.h"
#include "player.h"

const int startingGold = 99;
const int minCombatGold = 10;
const int maxCombatGold = 20;
const int minEliteGold = 25;
const int maxEliteGold = 35;
const int relicPrice = 150;
const int shopCardCount = 4;
const int noRelicGold = 50;

struct ShopStock
{
    std::vector<CardId> cards;
    std::vector<bool> cardSold;
    bool hasRelic = false;
    RelicId relic = RelicId::OakShield;
    bool relicSold = false;
};

struct RunState
{
    PlayerState player;
    int gold = startingGold;
    std::vector<RelicId> relics;

    DungeonMap map;
    int currentRoom = 0;

    std::vector<CardId> rewardChoices;
    int rewardGold = 0;
    bool hasRewardRelic = false;
    RelicId rewardRelic = RelicId::OakShield;
    ShopStock shop;
    EventId event = EventId::OldShrine;

    std::uint32_t seed = 0;
    std::mt19937 rng;
};
