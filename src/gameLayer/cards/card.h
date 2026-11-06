#pragma once

enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
};

class Card
{
public:
    const char *name = "";
    int cost = 0;
    int damage = 0;
    int block = 0;
};
