#pragma once
#include <vector>
#include "cards/card.h"

const int startingHp = 80;
const int restHeal = 20;
const int rewardChoiceCount = 3;

struct PlayerState
{
    int maxHp = startingHp;
    int hp = startingHp;
    std::vector<CardId> masterDeck;
};
