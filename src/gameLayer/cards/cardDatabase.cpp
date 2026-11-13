#include "cardDatabase.h"
#include <stdexcept>

const Card &GetCard(CardId id)
{
    // Definitions
    static const Card strike{"Strike", 1, Target::SingleEnemy, {{EffectType::Damage, 6}}};
    static const Card defend{"Defend", 1, Target::Self, {{EffectType::Block, 5}}};
    static const Card heavyStrike{"Heavy Strike", 2, Target::SingleEnemy, {{EffectType::Damage, 12}}};
    static const Card cleave{"Cleave", 1, Target::AllEnemies, {{EffectType::Damage, 8}}};

    // Lookup
    switch (id)
    {
    case CardId::Strike:
        return strike;
    case CardId::Defend:
        return defend;
    case CardId::HeavyStrike:
        return heavyStrike;
    case CardId::Cleave:
        return cleave;
    }

    throw std::out_of_range("Invalid CardId");
}

// Rewards
std::vector<CardId> RollRewards(std::mt19937 &rng, int count)
{
    std::vector<CardId> pool{CardId::Strike, CardId::Defend, CardId::HeavyStrike, CardId::Cleave};
    std::vector<CardId> choices;

    while (static_cast<int>(choices.size()) < count && !pool.empty())
    {
        std::uniform_int_distribution<int> pick(0, static_cast<int>(pool.size()) - 1);
        int index = pick(rng);
        choices.push_back(pool[index]);
        pool.erase(pool.begin() + index);
    }

    return choices;
}
