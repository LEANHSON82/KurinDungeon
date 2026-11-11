#include "cardEffect.h"
#include "damage.h"

static void ApplyEffect(const Effect &effect, EffectContext context)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        ApplyDamage(effect.value, context.enemy.hp, context.enemy.block);
        break;
    case EffectType::Block:
        context.playerBlock += effect.value;
        break;
    }
}

void ApplyCardEffects(const Card &card, EffectContext context)
{
    for (const Effect &effect : card.effects)
    {
        ApplyEffect(effect, context);
    }
}
