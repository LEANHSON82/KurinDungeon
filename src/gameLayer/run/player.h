#pragma once
#include <vector>
#include "cards/card.h"

class PlayerState
{
public:
    int maxHp = 80;
    int hp = 80;
    int gold = 0;

    std::vector<CardId> masterDeck{
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,

        CardId::Defend,
        CardId::Defend,
        CardId::Defend,
        CardId::Defend,

        CardId::HeavyStrike,
    };
};