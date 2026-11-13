#include "cardEffect.h"
#include "combat/damage.h"

// Single effect
static void ApplyEffect(const Effect &effect, const EffectContext &context)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        for (Enemy *enemy : context.enemies)
        {
            ApplyDamage(effect.value, enemy->hp, enemy->block);
        }
        break;
    case EffectType::Block:
        context.playerBlock += effect.value;
        break;
    }
}

// All effects
void ApplyCardEffects(const Card &card, const EffectContext &context)
{
    for (const Effect &effect : card.effects)
    {
        ApplyEffect(effect, context);
    }
}
