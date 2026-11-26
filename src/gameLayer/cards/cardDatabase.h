#pragma once
#include <random>
#include <vector>
#include "card.h"

const Card &GetCard(CardId id);
const std::vector<CardId> &GetStartingDeck();

std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
int GetCardPrice(CardId id);
