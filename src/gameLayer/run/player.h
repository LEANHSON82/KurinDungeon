#pragma once
#include <vector>
#include "cards/card.h"

const int restHeal = 20;

struct PlayerState
{
    int maxHp = 80;
    int hp = 80;

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
