#pragma once
#include <string>
#include "cards/card.h"

enum class RelicId
{
    FieldBandage,
    Whetstone,
    OakShield,
    IronBuckler,
    EmberStone,
    Satchel,
    HexAmulet,
    WarDrum,
    LuckyCoin,
    Count,
};

const int relicCount = static_cast<int>(RelicId::Count);

enum class RelicTrigger
{
    BattleStart,
    TurnStart,
    AttackPlayed,
    BattleWon,
};

struct Relic
{
    const char *name = "";
    const char *art = "";
    RelicTrigger trigger = RelicTrigger::BattleStart;
    Target target = Target::Self;
    Effect effect;
    bool starter = false;
};

std::string DescribeRelic(const Relic &relic);
