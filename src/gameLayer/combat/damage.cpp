#include "damage.h"
#include <algorithm>

void ApplyDamage(int amount, int &hp, int &block)
{
    if (amount <= 0)
    {
        return;
    }

    // Block then HP
    int blocked = std::min(block, amount);
    block -= blocked;
    hp = std::max(0, hp - (amount - blocked));
}
