#pragma once
#include <string>
#include <vector>
#include "combat/status.h"

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

const int cardCount = static_cast<int>(CardId::Count);

enum class CardKind
{
    Attack,
    Skill,
    Power,
};

enum class CardRarity
{
    Starter,
    Common,
    Uncommon,
    Rare,
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
    DrawCards,
    GainEnergy,
    LoseHp,
    ApplyStatus,
    Heal,
    GainGold,
};

struct Effect
{
    EffectType type = EffectType::Damage;
    int value = 0;
    StatusType status = StatusType::Strength;
};

struct Card
{
    const char *name = "";
    const char *art = "";
    CardKind kind = CardKind::Attack;
    CardRarity rarity = CardRarity::Common;
    int cost = 0;
    Target target = Target::Self;
    std::vector<Effect> effects;
    bool exhaust = false;
};

std::string DescribeEffect(const Effect &effect, Target target, const Statuses &source, const Statuses &victim);
std::string DescribeEffects(const Card &card, const Statuses &source, const Statuses &victim);
std::string DescribeEffects(const Card &card);
const char *GetKindName(CardKind kind);
const char *GetRarityName(CardRarity rarity);
