#pragma once
#include <vector>
#include "cards/card.h"

// Rest
const int restHeal = 20;

struct PlayerState
{
    // Health
    int maxHp = 80;
    int hp = 80;

    // Master deck
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
