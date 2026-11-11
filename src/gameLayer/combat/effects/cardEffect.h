#pragma once
#include "cards/card.h"
#include "combat/enemies/enemy.h"

// Targets
struct EffectContext
{
    Enemy &enemy;
    int &playerBlock;
};

// Apply effects
void ApplyCardEffects(const Card &card, EffectContext context);
