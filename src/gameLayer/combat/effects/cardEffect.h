#pragma once
#include <vector>
#include "cards/card.h"
#include "combat/enemies/enemy.h"

// Targets
struct EffectContext
{
    std::vector<Enemy *> enemies;
    int &playerBlock;
};

// Apply effects
void ApplyCardEffects(const Card &card, const EffectContext &context);
