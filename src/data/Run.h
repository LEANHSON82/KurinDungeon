#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "data/Cards.h"

inline constexpr int startingHp = 80;
inline constexpr int restHeal = 20;
inline constexpr int rewardChoiceCount = 3;

// What the player carries from room to room. GameEngine owns it; scenes read and change it.
struct Run
{
    std::uint32_t seed = 0;
    std::mt19937 rng;
    int hp = startingHp;
    int maxHp = startingHp;
    std::vector<CardId> deck;
    // Set by the Rest and Reward scenes; the Map scene then marks the current room cleared.
    bool roomFinished = false;
};
