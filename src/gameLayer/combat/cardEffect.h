#pragma once
#include "cards/card.h"
#include "enemy.h"

struct EffectContext
{
    Enemy &enemy;
    int &playerBlock;
};

void ApplyCardEffects(const Card &card, EffectContext context);
