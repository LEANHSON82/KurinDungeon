#include "cardEffect.h"
#include "combat/damage.h"

static void DamageTargets(int amount, EffectContext &context)
{
    for (int index : context.targets)
    {
        Enemy &enemy = context.enemies[index];
        int hpBefore = enemy.hp;
        ApplyDamage(amount, enemy.hp, enemy.block);

        context.events.push_back({BattleEventType::Damage, playerActor, index, hpBefore - enemy.hp});
        if (enemy.hp == 0)
        {
            context.events.push_back({BattleEventType::Death, playerActor, index, 0});
        }
    }
}

static void ApplyEffect(const Effect &effect, EffectContext &context)
{
    switch (effect.type)
    {
    case EffectType::Damage:
        DamageTargets(effect.value, context);
        break;
    case EffectType::Block:
        context.playerBlock += effect.value;
        context.events.push_back({BattleEventType::Block, playerActor, playerActor, effect.value});
        break;
    }
}

void ApplyCardEffects(const Card &card, EffectContext &context)
{
    for (const Effect &effect : card.effects)
    {
        ApplyEffect(effect, context);
    }
}
