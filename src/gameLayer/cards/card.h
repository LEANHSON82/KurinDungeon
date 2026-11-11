#pragma once
#include <string>
#include <vector>

// Card IDs
enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
};

// Effect types
enum class EffectType
{
    Damage,
    Block,
};

// Effect data
struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
};

// Card data
struct Card
{
    const char *name = "";
    int cost = 0;
    std::vector<Effect> effects;
};

// Description
std::string DescribeEffects(const Card &card);
