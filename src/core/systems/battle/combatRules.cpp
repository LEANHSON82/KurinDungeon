#include "core/systems/battle/combatRules.h"
#include <algorithm>
#include "core/components/combat.h"
#include "core/state/run.h"

ecs::Entity GetPlayer(const ecs::Registry &registry)
{
    return registry.Resource<RunState>().player;
}

std::vector<ecs::Entity> GetEnemiesInOrder(const ecs::Registry &registry)
{
    std::vector<ecs::Entity> enemies = registry.View<Enemy>().Collect();
    std::sort(enemies.begin(), enemies.end(), [&](ecs::Entity a, ecs::Entity b) {
        return registry.Get<Enemy>(a).slot < registry.Get<Enemy>(b).slot;
    });
    return enemies;
}

std::vector<ecs::Entity> GetPile(const ecs::Registry &registry, Zone zone)
{
    std::vector<ecs::Entity> pile;
    registry.View<InZone>().Each([&](ecs::Entity card, const InZone &inZone) {
        if (inZone.zone == zone)
        {
            pile.push_back(card);
        }
    });
    std::sort(pile.begin(), pile.end(), [&](ecs::Entity a, ecs::Entity b) {
        return registry.Get<InZone>(a).order < registry.Get<InZone>(b).order;
    });
    return pile;
}

bool IsEnemyAlive(const ecs::Registry &registry, ecs::Entity entity)
{
    return registry.Valid(entity) && registry.Has<Enemy>(entity) && !registry.Has<Dead>(entity);
}

bool CanPlayCard(const ecs::Registry &registry, ecs::Entity card, ecs::Entity target)
{
    const BattleState &battle = registry.Resource<BattleState>();
    if (!battle.active || battle.result != BattleResult::InProgress || !registry.Valid(card))
    {
        return false;
    }

    const InZone *inZone = registry.TryGet<InZone>(card);
    if (inZone == nullptr || inZone->zone != Zone::Hand)
    {
        return false;
    }

    const Card &data = GetCard(registry.Get<DeckCard>(card).id);
    if (data.target == Target::SingleEnemy && !IsEnemyAlive(registry, target))
    {
        return false;
    }

    return battle.energy >= data.cost;
}

void PushEvent(ecs::Registry &registry, BattleEventType type, ecs::Entity source, ecs::Entity target, int value)
{
    registry.Resource<BattleEvents>().pending.push_back({type, source, target, value});
}

void DealDamage(ecs::Registry &registry, ecs::Entity source, ecs::Entity target, int amount)
{
    Health &health = registry.Get<Health>(target);
    Block &block = registry.Get<Block>(target);
    int hpBefore = health.hp;

    if (amount > 0)
    {
        int blocked = std::min(block.value, amount);
        block.value -= blocked;
        health.hp = std::max(0, health.hp - (amount - blocked));
    }

    PushEvent(registry, BattleEventType::Damage, source, target, hpBefore - health.hp);
    if (health.hp == 0 && !registry.Has<Dead>(target))
    {
        registry.Emplace<Dead>(target);
        PushEvent(registry, BattleEventType::Death, source, target, 0);
    }
}

void GainBlock(ecs::Registry &registry, ecs::Entity source, ecs::Entity target, int amount)
{
    registry.Get<Block>(target).value += amount;
    PushEvent(registry, BattleEventType::Block, source, target, amount);
}

void UpdateBattleResult(ecs::Registry &registry)
{
    BattleState &battle = registry.Resource<BattleState>();
    if (registry.Get<Health>(GetPlayer(registry)).hp <= 0)
    {
        battle.result = BattleResult::Lost;
        return;
    }

    bool anyAlive = false;
    registry.View<Enemy>().Each([&](ecs::Entity enemy, const Enemy &) {
        anyAlive = anyAlive || !registry.Has<Dead>(enemy);
    });
    if (!anyAlive)
    {
        battle.result = BattleResult::Won;
    }
}
