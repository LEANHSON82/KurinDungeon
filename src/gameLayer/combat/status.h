#pragma once

enum class StatusType
{
    Strength,
    Weak,
    Vulnerable,
    Count,
};

const int statusCount = static_cast<int>(StatusType::Count);

struct Statuses
{
    int strength = 0;
    int weak = 0;
    int vulnerable = 0;
};

struct StatusDefinition
{
    const char *name = "";
    bool countsDown = false;
    bool harmful = false;
};

const StatusDefinition &GetStatusDefinition(StatusType type);
int GetStatus(const Statuses &statuses, StatusType type);
void AddStatus(Statuses &statuses, StatusType type, int amount);
