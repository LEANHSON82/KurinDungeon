#include "data/Statuses.h"
#include <cassert>
#include <iterator>

static const StatusDefinition statuses[]{
    {"Strength", "Star", false, false},
    {"Weak", "StatusWeak", true, true},
    {"Vulnerable", "StatusVulnerable", true, true},
};
static_assert(std::size(statuses) == statusCount, "Every StatusType needs an entry in the statuses table");

const StatusDefinition &GetStatusDefinition(StatusType type)
{
    int index = static_cast<int>(type);
    assert(index >= 0 && index < statusCount && "Invalid StatusType");
    return statuses[index];
}
