#pragma once
#include <random>
#include <vector>
#include "card.h"

// Lookup
const Card &GetCard(CardId id);

// Rewards
std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
