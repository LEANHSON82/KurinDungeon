#pragma once
#include <random>
#include <vector>
#include "card.h"

const Card &GetCard(CardId id);

std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
