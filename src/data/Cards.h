#pragma once
#include <random>
#include <string>
#include <vector>

// Keep in the same order as the table in Cards.cpp.
enum class CardId
{
    Strike,
    Defend,
    HeavyStrike,
    Cleave,
    QuickSlash,
    DoubleCut,
    RecklessBlow,
    Sweep,
    GuardedStrike,
    Flurry,
    Execute,
    IronWall,
    Brace,
    BattlePlan,
    Surge,
    DarkPact,
    Count,
};

inline constexpr int cardCount = static_cast<int>(CardId::Count);

enum class CardKind
{
    Attack,
    Skill,
    // Lasting effects arrive with statuses; a played Power leaves the deck for the battle.
    Power,
};

enum class Target
{
    Self,
    SingleEnemy,
    AllEnemies,
};

// Damage hits the card's targets. Every other effect applies to the player.
enum class EffectType
{
    Damage,
    Block,
    DrawCards,
    GainEnergy,
    LoseHp, // lost straight from HP, ignoring Block
};

struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
};

struct Card
{
    const char *name = "";
    const char *art = ""; // texture name in assets.txt
    CardKind kind = CardKind::Attack;
    int cost = 0;
    Target target = Target::Self;
    std::vector<Effect> effects;
    bool exhaust = false; // exhausted cards leave the deck until the battle ends
    bool rewardable = true;
};

const Card &GetCard(CardId id);
std::string DescribeEffects(const Card &card);
const char *GetKindName(CardKind kind);

const std::vector<CardId> &GetStartingDeck();
std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
