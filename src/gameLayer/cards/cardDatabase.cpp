#include "cardDatabase.h"
#include <algorithm>

const bool exhausts = true;
const int commonWeight = 60;
const int uncommonWeight = 30;
const int rareWeight = 10;

static const Card cards[cardCount]{
    {"Strike", "images/icons/spells/199_power_strike.png", CardKind::Attack, CardRarity::Starter, 1, Target::SingleEnemy, {{EffectType::Damage, 6}}},
    {"Defend", "images/icons/spells/112_holy_shield.png", CardKind::Skill, CardRarity::Starter, 1, Target::Self, {{EffectType::Block, 5}}},
    {"Heavy Strike", "images/icons/spells/208_finishing_blow.png", CardKind::Attack, CardRarity::Common, 2, Target::SingleEnemy, {{EffectType::Damage, 12}}},
    {"Cleave", "images/icons/spells/200_whirlwind.png", CardKind::Attack, CardRarity::Common, 1, Target::AllEnemies, {{EffectType::Damage, 8}}},
    {"Quick Slash", "images/icons/spells/049_wind_slash.png", CardKind::Attack, CardRarity::Common, 1, Target::SingleEnemy, {{EffectType::Damage, 5}, {EffectType::DrawCards, 1}}},
    {"Double Cut", "images/icons/spells/202_counterattack.png", CardKind::Attack, CardRarity::Common, 1, Target::SingleEnemy, {{EffectType::Damage, 4}, {EffectType::Damage, 4}}},
    {"Reckless Blow", "images/icons/spells/207_berserker_s_rage.png", CardKind::Attack, CardRarity::Uncommon, 1, Target::SingleEnemy, {{EffectType::LoseHp, 3}, {EffectType::Damage, 15}}},
    {"Sweep", "images/icons/spells/203_shocking_wave.png", CardKind::Attack, CardRarity::Uncommon, 2, Target::AllEnemies, {{EffectType::Damage, 12}}},
    {"Guarded Strike", "images/icons/spells/206_parry.png", CardKind::Attack, CardRarity::Common, 1, Target::SingleEnemy, {{EffectType::Damage, 5}, {EffectType::Block, 5}}},
    {"Flurry", "images/icons/spells/045_lightning_bolt.png", CardKind::Attack, CardRarity::Common, 0, Target::SingleEnemy, {{EffectType::Damage, 3}}},
    {"Execute", "images/icons/spells/186_final_judgement.png", CardKind::Attack, CardRarity::Rare, 2, Target::SingleEnemy, {{EffectType::Damage, 22}}, exhausts},
    {"Iron Wall", "images/icons/spells/069_stone_armor.png", CardKind::Skill, CardRarity::Uncommon, 2, Target::Self, {{EffectType::Block, 13}}},
    {"Brace", "images/icons/spells/091_arcane_shield.png", CardKind::Skill, CardRarity::Common, 1, Target::Self, {{EffectType::Block, 7}, {EffectType::DrawCards, 1}}},
    {"Battle Plan", "images/icons/spells/135_ancestral_guidance.png", CardKind::Skill, CardRarity::Uncommon, 1, Target::Self, {{EffectType::DrawCards, 2}}},
    {"Surge", "images/icons/spells/160_haste.png", CardKind::Skill, CardRarity::Uncommon, 0, Target::Self, {{EffectType::GainEnergy, 2}}, exhausts},
    {"Dark Pact", "images/icons/spells/178_blood_pact.png", CardKind::Skill, CardRarity::Rare, 0, Target::Self, {{EffectType::LoseHp, 4}, {EffectType::DrawCards, 3}}, exhausts},
    {"Crushing Blow", "images/icons/spells/053_thunder_smash.png", CardKind::Attack, CardRarity::Uncommon, 2, Target::SingleEnemy,
     {{EffectType::Damage, 8}, {EffectType::ApplyStatus, 2, StatusType::Vulnerable}}},
    {"Menace", "images/icons/spells/180_curse.png", CardKind::Skill, CardRarity::Uncommon, 1, Target::AllEnemies, {{EffectType::ApplyStatus, 1, StatusType::Weak}}},
    {"Battle Cry", "images/icons/spells/204_battlecry.png", CardKind::Power, CardRarity::Rare, 1, Target::Self, {{EffectType::ApplyStatus, 2, StatusType::Strength}}},
};

const Card &GetCard(CardId id)
{
    return cards[static_cast<int>(id)];
}

const std::vector<CardId> &GetStartingDeck()
{
    static const std::vector<CardId> deck{
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,
        CardId::Strike,

        CardId::Defend,
        CardId::Defend,
        CardId::Defend,
        CardId::Defend,

        CardId::HeavyStrike,
    };
    return deck;
}

static CardRarity RollRarity(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> roll(1, commonWeight + uncommonWeight + rareWeight);
    int value = roll(rng);
    if (value <= commonWeight)
    {
        return CardRarity::Common;
    }
    if (value <= commonWeight + uncommonWeight)
    {
        return CardRarity::Uncommon;
    }
    return CardRarity::Rare;
}

static std::vector<CardId> GetCardsOfRarity(CardRarity rarity, const std::vector<CardId> &taken)
{
    std::vector<CardId> pool;
    for (int i = 0; i < cardCount; i++)
    {
        CardId id = static_cast<CardId>(i);
        bool alreadyTaken = std::find(taken.begin(), taken.end(), id) != taken.end();
        if (GetCard(id).rarity == rarity && !alreadyTaken)
        {
            pool.push_back(id);
        }
    }
    return pool;
}

std::vector<CardId> RollRewards(std::mt19937 &rng, int count)
{
    std::vector<CardId> choices;
    for (int i = 0; i < count; i++)
    {
        std::vector<CardId> pool = GetCardsOfRarity(RollRarity(rng), choices);
        if (pool.empty())
        {
            pool = GetCardsOfRarity(CardRarity::Common, choices);
        }
        if (pool.empty())
        {
            break;
        }

        std::uniform_int_distribution<int> pick(0, static_cast<int>(pool.size()) - 1);
        choices.push_back(pool[pick(rng)]);
    }

    return choices;
}

int GetCardPrice(CardId id)
{
    switch (GetCard(id).rarity)
    {
    case CardRarity::Starter:
    case CardRarity::Common:
        return 50;
    case CardRarity::Uncommon:
        return 75;
    case CardRarity::Rare:
        return 140;
    }
    return 50;
}
