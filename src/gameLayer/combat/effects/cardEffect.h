#pragma once
#include <vector>
#include "cards/card.h"
#include "combat/battleEvent.h"
#include "combat/enemies/enemy.h"

struct EffectContext
{
    std::vector<Enemy> &enemies;
    std::vector<int> targets;
    int &playerBlock;
    std::vector<BattleEvent> &events;
};

void ApplyCardEffects(const Card &card, EffectContext &context);
