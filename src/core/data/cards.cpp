#include "core/data/cards.h"
#include <stdexcept>

const Card &GetCard(CardId id)
{
    static const Card strike{"Strike", 1, Target::SingleEnemy, {{EffectType::Damage, 6}}};
    static const Card defend{"Defend", 1, Target::Self, {{EffectType::Block, 5}}};
    static const Card heavyStrike{"Heavy Strike", 2, Target::SingleEnemy, {{EffectType::Damage, 12}}};
    static const Card cleave{"Cleave", 1, Target::AllEnemies, {{EffectType::Damage, 8}}};

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

static const char *GetEffectName(EffectType type)
{
    switch (type)
    {
    case EffectType::Damage:
        return "Damage";
    case EffectType::Block:
        return "Block";
    }

    return "Unknown";
}

std::string DescribeEffects(const Card &card)
{
    std::string text;
    for (const Effect &effect : card.effects)
    {
        if (!text.empty())
        {
            text += ", ";
        }
        text += GetEffectName(effect.type);
        text += ' ';
        text += std::to_string(effect.value);
    }
    if (card.target == Target::AllEnemies)
    {
        text += " to all";
    }
    return text;
}

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
