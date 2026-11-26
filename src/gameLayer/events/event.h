#pragma once
#include <vector>

enum class EventId
{
    OldShrine,
    AbandonedCamp,
    LostScroll,
    Count,
};

const int eventCount = static_cast<int>(EventId::Count);

enum class OutcomeType
{
    LoseHp,
    Heal,
    GainGold,
    GainRelic,
    GainCard,
};

struct Outcome
{
    OutcomeType type = OutcomeType::GainGold;
    int value = 0;
};

struct EventChoice
{
    const char *label = "";
    std::vector<Outcome> outcomes;
};

struct Event
{
    const char *name = "";
    const char *text = "";
    std::vector<EventChoice> choices;
};
