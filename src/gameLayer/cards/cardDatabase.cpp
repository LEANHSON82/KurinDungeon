#include "cardDatabase.h"
#include <stdexcept>

const Card &GetCard(CardId id)
{
    static const Card strike{"Strike", 1, 6, 0};
    static const Card defend{"Defend", 1, 0, 5};
    static const Card heavyStrike{"Heavy Strike", 2, 12, 0};

    switch (id)
    {
    case CardId::Strike:
        return strike;
    case CardId::Defend:
        return defend;
    case CardId::HeavyStrike:
        return heavyStrike;
    }

    throw std::out_of_range("Invalid CardId");
}
