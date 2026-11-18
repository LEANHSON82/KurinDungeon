#pragma once
#include <string>
#include <vector>

// Keep in the same order as the table in cards.cpp.
enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
    Cleave,
    Count,
};

inline constexpr int cardCount = static_cast<int>(CardId::Count);

enum class CardKind
{
    Attack,
    Skill,
};

enum class Target
{
    Self,
    SingleEnemy,
    AllEnemies,
};

enum class EffectType
{
    Damage,
    Block,
};

// An effect applies to every target of its card.
struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
};

struct Card
{
    const char *name = "";
    CardKind kind = CardKind::Attack;
    int cost = 0;
    Target target = Target::Self;
    std::vector<Effect> effects;
    bool rewardable = true;
};

const Card &GetCard(CardId id);

std::string DescribeEffects(const Card &card);

const std::vector<CardId> &GetStartingDeck();
