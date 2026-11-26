#include "relic.h"

static const char *GetTriggerText(RelicTrigger trigger)
{
    switch (trigger)
    {
    case RelicTrigger::BattleStart:
        return "Start of battle: ";
    case RelicTrigger::TurnStart:
        return "Start of each turn: ";
    case RelicTrigger::AttackPlayed:
        return "When you play an Attack: ";
    case RelicTrigger::BattleWon:
        return "After winning a battle: ";
    }
    return "";
}

std::string DescribeRelic(const Relic &relic)
{
    return GetTriggerText(relic.trigger) + DescribeEffect(relic.effect, relic.target, Statuses{}, Statuses{});
}
