#pragma once
#include <random>
#include <vector>
#include "core/data/cards.h"

std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
