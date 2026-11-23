#pragma once

// Keep in the same order as the table in Statuses.cpp.
enum class StatusType
{
    Strength,   // adds to every hit the owner deals; lasts the whole battle
    Weak,       // the owner deals 25% less damage; counts down each round
    Vulnerable, // the owner takes 50% more damage; counts down each round
    Count,
};

inline constexpr int statusCount = static_cast<int>(StatusType::Count);

struct StatusDefinition
{
    const char *name = "";
    const char *icon = ""; // texture name in assets.txt
    bool countsDown = false; // loses one point at the end of every round, after the enemies act
    bool harmful = false;
};

const StatusDefinition &GetStatusDefinition(StatusType type);
