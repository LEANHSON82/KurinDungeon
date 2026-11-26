#include "relicDatabase.h"
#include <algorithm>

const bool starterRelic = true;

static const Relic relics[relicCount]{
    {"Field Bandage", "images/icons/base/168_heal_2.png", RelicTrigger::BattleWon, Target::Self, {EffectType::Heal, 5}, starterRelic},
    {"Whetstone", "images/icons/base/112_stone.png", RelicTrigger::BattleStart, Target::Self, {EffectType::ApplyStatus, 1, StatusType::Strength}, starterRelic},
    {"Oak Shield", "images/icons/base/045_wooden_shield.png", RelicTrigger::BattleStart, Target::Self, {EffectType::Block, 8}},
    {"Iron Buckler", "images/icons/base/064_steel_buckler.png", RelicTrigger::TurnStart, Target::Self, {EffectType::Block, 2}},
    {"Ember Stone", "images/icons/base/096_ruby.png", RelicTrigger::BattleStart, Target::Self, {EffectType::GainEnergy, 1}},
    {"Satchel", "images/icons/base/103_satchel.png", RelicTrigger::BattleStart, Target::Self, {EffectType::DrawCards, 2}},
    {"Hex Amulet", "images/icons/base/080_ice_necklace.png", RelicTrigger::BattleStart, Target::AllEnemies, {EffectType::ApplyStatus, 1, StatusType::Vulnerable}},
    {"War Drum", "images/icons/base/293_drum_1.png", RelicTrigger::AttackPlayed, Target::Self, {EffectType::Block, 1}},
    {"Lucky Coin", "images/icons/base/272_coin_silver.png", RelicTrigger::BattleWon, Target::Self, {EffectType::GainGold, 15}},
};

const Relic &GetRelic(RelicId id)
{
    return relics[static_cast<int>(id)];
}

bool RollRelic(std::mt19937 &rng, const std::vector<RelicId> &owned, RelicId &relic)
{
    std::vector<RelicId> pool;
    for (int i = 0; i < relicCount; i++)
    {
        RelicId id = static_cast<RelicId>(i);
        bool alreadyOwned = std::find(owned.begin(), owned.end(), id) != owned.end();
        if (!GetRelic(id).starter && !alreadyOwned)
        {
            pool.push_back(id);
        }
    }
    if (pool.empty())
    {
        return false;
    }

    std::uniform_int_distribution<int> pick(0, static_cast<int>(pool.size()) - 1);
    relic = pool[pick(rng)];
    return true;
}
