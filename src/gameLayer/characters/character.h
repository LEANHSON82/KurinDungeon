#pragma once
#include <vector>
#include "cards/card.h"
#include "relics/relic.h"

enum class CharacterId
{
    Knight,
    Swordsman,
    Count,
};

const int characterCount = static_cast<int>(CharacterId::Count);

struct CharacterDefinition
{
    const char *name = "";
    int maxHp = 0;
    std::vector<CardId> startingDeck;
    RelicId starterRelic = RelicId::FieldBandage;
};
