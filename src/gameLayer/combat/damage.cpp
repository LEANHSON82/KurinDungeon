#include "damage.h"
#include <algorithm>

const int weakNumerator = 3;
const int weakDenominator = 4;
const int vulnerableNumerator = 3;
const int vulnerableDenominator = 2;

int ModifyDamage(int base, const Statuses &source, const Statuses &target)
{
    int damage = base + source.strength;
    int numerator = 1;
    int denominator = 1;
    if (source.weak > 0)
    {
        numerator *= weakNumerator;
        denominator *= weakDenominator;
    }
    if (target.vulnerable > 0)
    {
        numerator *= vulnerableNumerator;
        denominator *= vulnerableDenominator;
    }
    return std::max(0, damage) * numerator / denominator;
}

void ApplyDamage(int amount, int &hp, int &block)
{
    if (amount <= 0)
    {
        return;
    }

    int blocked = std::min(block, amount);
    block -= blocked;
    hp = std::max(0, hp - (amount - blocked));
}
