#pragma once
#include <string>
#include <vector>

enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
};

enum class EffectType
{
    Damage,
    Block,
};

struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
};

struct Card
{
    const char *name = "";
    int cost = 0;
    std::vector<Effect> effects;
};

std::string DescribeEffects(const Card &card);
