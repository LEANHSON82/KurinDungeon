#include "cardEffect.h"
#include "damage.h"

static void ApplyEffect(const Effect &effect, Enemy &enemy, int &playerBlock)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        ApplyDamage(effect.value, enemy.hp, enemy.block);
        break;
    case EffectType::Block:
        playerBlock += effect.value;
        break;
    }
}

void ApplyCardEffects(const Card &card, Enemy &enemy, int &playerBlock)
{
    for (const Effect &effect : card.effects)
    {
        ApplyEffect(effect, enemy, playerBlock);
    }
}
