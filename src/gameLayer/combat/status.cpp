#include "status.h"

const StatusDefinition &GetStatusDefinition(StatusType type)
{
    static const StatusDefinition statuses[statusCount]{
        {"Strength", false, false},
        {"Weak", true, true},
        {"Vulnerable", true, true},
    };
    return statuses[static_cast<int>(type)];
}

int GetStatus(const Statuses &statuses, StatusType type)
{
    switch (type)
    {
    case StatusType::Strength:
        return statuses.strength;
    case StatusType::Weak:
        return statuses.weak;
    case StatusType::Vulnerable:
        return statuses.vulnerable;
    case StatusType::Count:
        break;
    }
    return 0;
}

void AddStatus(Statuses &statuses, StatusType type, int amount)
{
    switch (type)
    {
    case StatusType::Strength:
        statuses.strength += amount;
        break;
    case StatusType::Weak:
        statuses.weak += amount;
        break;
    case StatusType::Vulnerable:
        statuses.vulnerable += amount;
        break;
    case StatusType::Count:
        break;
    }
}
