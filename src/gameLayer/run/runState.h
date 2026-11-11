#pragma once
#include <cstdint>
#include <random>
#include "player.h"

struct RunState
{
    PlayerState player;
    std::uint32_t seed = 0;
    std::mt19937 rng;
};
