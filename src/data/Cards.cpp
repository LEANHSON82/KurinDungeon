#include "data/Cards.h"
#include <cassert>
#include <iterator>

static constexpr bool exhausts = true;
static constexpr bool notRewardable = false;

// Columns: name, art, kind, cost, target, effects, exhaust, rewardable.
// The last two are left out for an ordinary card: it does not exhaust and can be a reward.
static const Card cards[]{
    {"Strike", "CardStrike", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::Damage, 6}}, false, notRewardable},
    {"Defend", "CardDefend", CardKind::Skill, 1, Target::Self, {{EffectType::Block, 5}}, false, notRewardable},
    {"Heavy Strike", "CardHeavyStrike", CardKind::Attack, 2, Target::SingleEnemy, {{EffectType::Damage, 12}}},
    {"Cleave", "CardCleave", CardKind::Attack, 1, Target::AllEnemies, {{EffectType::Damage, 8}}},
    {"Quick Slash", "CardQuickSlash", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::Damage, 5}, {EffectType::DrawCards, 1}}},
    {"Double Cut", "CardDoubleCut", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::Damage, 4}, {EffectType::Damage, 4}}},
    {"Reckless Blow", "CardRecklessBlow", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::LoseHp, 3}, {EffectType::Damage, 15}}},
    {"Sweep", "CardSweep", CardKind::Attack, 2, Target::AllEnemies, {{EffectType::Damage, 12}}},
    {"Guarded Strike", "CardGuardedStrike", CardKind::Attack, 1, Target::SingleEnemy, {{EffectType::Damage, 5}, {EffectType::Block, 5}}},
    {"Flurry", "CardFlurry", CardKind::Attack, 0, Target::SingleEnemy, {{EffectType::Damage, 3}}},
    {"Execute", "CardExecute", CardKind::Attack, 2, Target::SingleEnemy, {{EffectType::Damage, 22}}, exhausts},
    {"Iron Wall", "CardIronWall", CardKind::Skill, 2, Target::Self, {{EffectType::Block, 13}}},
    {"Brace", "CardBrace", CardKind::Skill, 1, Target::Self, {{EffectType::Block, 7}, {EffectType::DrawCards, 1}}},
    {"Battle Plan", "CardBattlePlan", CardKind::Skill, 1, Target::Self, {{EffectType::DrawCards, 2}}},
    {"Surge", "CardSurge", CardKind::Skill, 0, Target::Self, {{EffectType::GainEnergy, 2}}, exhausts},
    {"Dark Pact", "CardDarkPact", CardKind::Skill, 0, Target::Self, {{EffectType::LoseHp, 4}, {EffectType::DrawCards, 3}}, exhausts},
    {"Crushing Blow", "CardCrushingBlow", CardKind::Attack, 2, Target::SingleEnemy,
     {{EffectType::Damage, 8}, {EffectType::ApplyStatus, 2, StatusType::Vulnerable}}},
    {"Menace", "CardMenace", CardKind::Skill, 1, Target::AllEnemies, {{EffectType::ApplyStatus, 1, StatusType::Weak}}},
    {"Battle Cry", "CardBattleCry", CardKind::Power, 1, Target::Self, {{EffectType::ApplyStatus, 2, StatusType::Strength}}},
};
static_assert(std::size(cards) == cardCount, "Every CardId needs an entry in the cards table");

const Card &GetCard(CardId id)
{
    int index = static_cast<int>(id);
    assert(index >= 0 && index < cardCount && "Invalid CardId");
    return cards[index];
}

static std::string DescribeEffect(const Effect &effect, Target target, const std::function<int(int)> &damage)
{
    std::string value = std::to_string(effect.value);
    const char *toAll = target == Target::AllEnemies ? " to all" : "";
    switch (effect.type)
    {
    case EffectType::Damage:
        return "Damage " + std::to_string(damage ? damage(effect.value) : effect.value) + toAll;
    case EffectType::Block:
        return "Block " + value;
    case EffectType::DrawCards:
        return "Draw " + value;
    case EffectType::GainEnergy:
        return "+" + value + " Energy";
    case EffectType::LoseHp:
        return "Lose " + value + " HP";
    case EffectType::ApplyStatus:
    {
        std::string name = GetStatusDefinition(effect.status).name;
        if (target == Target::Self)
        {
            return "Gain " + value + " " + name;
        }
        return name + " " + value + toAll;
    }
    }
    return "";
}

std::string DescribeEffects(const Card &card, const std::function<int(int)> &damage)
{
    std::string text;
    for (const Effect &effect : card.effects)
    {
        if (!text.empty())
        {
            text += ", ";
        }
        text += DescribeEffect(effect, card.target, damage);
    }
    if (card.exhaust)
    {
        text += text.empty() ? "Exhaust" : ", Exhaust";
    }
    return text;
}

const char *GetKindName(CardKind kind)
{
    switch (kind)
    {
    case CardKind::Attack:
        return "Attack";
    case CardKind::Skill:
        return "Skill";
    case CardKind::Power:
        return "Power";
    }
    return "";
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

std::vector<CardId> RollRewards(std::mt19937 &rng, int count)
{
    std::vector<CardId> pool;
    for (int i = 0; i < cardCount; i++)
    {
        CardId id = static_cast<CardId>(i);
        if (GetCard(id).rewardable)
        {
            pool.push_back(id);
        }
    }

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
