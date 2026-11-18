#include "core/data/cards.h"
#include <cassert>
#include <iterator>

static const Card cards[]{
    {"Strike", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::Damage, 6}}},
    {"Defend", CardKind::Skill, 1, Target::Self, {{EffectType::Block, 5}}},
    {"Heavy Strike", CardKind::Attack, 2, Target::SingleEnemy, {{EffectType::Damage, 12}}},
    {"Cleave", CardKind::Attack, 1, Target::AllEnemies, {{EffectType::Damage, 8}}},
};
static_assert(std::size(cards) == cardCount, "Every CardId needs an entry in the cards table");

const Card &GetCard(CardId id)
{
    int index = static_cast<int>(id);
    assert(index >= 0 && index < cardCount && "Invalid CardId");
    return cards[index];
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

const std::vector<CardId> &GetStartingDeck()
{
    static const std::vector<CardId> deck{
        CardId::Strike, CardId::Strike, CardId::Strike, CardId::Strike, CardId::Strike,
        CardId::Defend, CardId::Defend, CardId::Defend, CardId::Defend,
        CardId::HeavyStrike,
    };
    return deck;
}
