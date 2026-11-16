#pragma once
#include "core/data/cards.h"

struct DeckCard
{
    CardId id = CardId::Strike;
};

enum class Zone
{
    Draw,
    Hand,
    Discard,
};

struct InZone
{
    Zone zone = Zone::Draw;
    int order = 0;
};
