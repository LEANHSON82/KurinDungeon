#pragma once
#include <vector>
#include "cards/card.h"
#include "characters/character.h"

const int restHeal = 20;
const int rewardChoiceCount = 3;

struct PlayerState
{
    CharacterId character = CharacterId::Knight;
    int maxHp = 0;
    int hp = 0;
    std::vector<CardId> masterDeck;
};
