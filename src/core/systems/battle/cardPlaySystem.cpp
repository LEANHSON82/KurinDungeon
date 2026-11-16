#include "core/systems/battle/cardPlaySystem.h"
#include "core/components/cards.h"
#include "core/state/battle.h"
#include "core/systems/battle/combatRules.h"
#include "core/systems/battle/deckSystem.h"

static std::vector<ecs::Entity> GetTargets(const ecs::Registry &registry, const Card &card, ecs::Entity target)
{
    std::vector<ecs::Entity> targets;
    switch (card.target)
    {
    case Target::Self:
        break;
    case Target::SingleEnemy:
        targets.push_back(target);
        break;
    case Target::AllEnemies:
        for (ecs::Entity enemy : GetEnemiesInOrder(registry))
        {
            if (IsEnemyAlive(registry, enemy))
            {
                targets.push_back(enemy);
            }
        }
        break;
    }
    return targets;
}

bool PlayCard(ecs::Registry &registry, ecs::Entity card, ecs::Entity target)
{
    if (!CanPlayCard(registry, card, target))
    {
        return false;
    }

    const Card &data = GetCard(registry.Get<DeckCard>(card).id);
    registry.Resource<BattleState>().energy -= data.cost;

    ecs::Entity player = GetPlayer(registry);
    std::vector<ecs::Entity> targets = GetTargets(registry, data, target);
    for (const Effect &effect : data.effects)
    {
        switch (effect.type)
        {
        case EffectType::Damage:
            for (ecs::Entity enemy : targets)
            {
                DealDamage(registry, player, enemy, effect.value);
            }
            break;
        case EffectType::Block:
            GainBlock(registry, player, player, effect.value);
            break;
        }
    }

    DiscardCard(registry, card);
    UpdateBattleResult(registry);
    return true;
}
