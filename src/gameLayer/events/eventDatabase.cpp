#include "eventDatabase.h"

static const Event events[eventCount]{
    {"Old Shrine", "A cracked shrine hums in the dark. It wants something from you.",
     {{"Offer blood: lose 8 HP, gain a relic", {{OutcomeType::LoseHp, 8}, {OutcomeType::GainRelic, 0}}},
      {"Walk away", {}}}},
    {"Abandoned Camp", "A cold campfire and a pile of packs. Nobody is coming back.",
     {{"Rest by the fire: heal 12 HP", {{OutcomeType::Heal, 12}}},
      {"Search the packs: gain 40 gold", {{OutcomeType::GainGold, 40}}}}},
    {"Lost Scroll", "A scroll lies on a broken table, its ink still fresh.",
     {{"Study it: gain a random card", {{OutcomeType::GainCard, 0}}},
      {"Sell it later: gain 25 gold", {{OutcomeType::GainGold, 25}}}}},
};

const Event &GetEvent(EventId id)
{
    return events[static_cast<int>(id)];
}

EventId RollEvent(std::mt19937 &rng)
{
    std::uniform_int_distribution<int> pick(0, eventCount - 1);
    return static_cast<EventId>(pick(rng));
}
