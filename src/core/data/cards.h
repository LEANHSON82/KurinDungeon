#pragma once
#include <random>
#include <string>
#include <vector>

enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
    Cleave,
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

struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
};

struct Card
{
    const char *name = "";
    int cost = 0;
    Target target = Target::Self;
    std::vector<Effect> effects;
};

const Card &GetCard(CardId id);

std::string DescribeEffects(const Card &card);

std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
