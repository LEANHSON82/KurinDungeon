#pragma once
#include <functional>
#include <random>
#include <string>
#include <vector>
#include "data/Statuses.h"

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
    CrushingBlow,
    Menace,
    BattleCry,
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

// Damage hits the card's targets. ApplyStatus hits the card's targets too, or the player
// when the card targets Self. Every other effect applies to the player.
enum class EffectType
{
    Damage,
    Block,
    DrawCards,
    GainEnergy,
    LoseHp, // lost straight from HP, ignoring Block
    ApplyStatus,
};

struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
    StatusType status = StatusType::Strength; // only used by ApplyStatus
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
// damage, when given, turns a card's base damage into the number the player will really deal
// (Strength, Weak, Vulnerable), so the text matches the result.
std::string DescribeEffects(const Card &card, const std::function<int(int)> &damage = nullptr);
const char *GetKindName(CardKind kind);

const std::vector<CardId> &GetStartingDeck();
std::vector<CardId> RollRewards(std::mt19937 &rng, int count);
