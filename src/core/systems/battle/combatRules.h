#pragma once
#include <vector>
#include "core/components/cards.h"
#include "core/state/battle.h"
#include "ecs/registry.h"

ecs::Entity GetPlayer(const ecs::Registry &registry);
std::vector<ecs::Entity> GetEnemiesInOrder(const ecs::Registry &registry);
std::vector<ecs::Entity> GetPile(const ecs::Registry &registry, Zone zone);

bool IsEnemyAlive(const ecs::Registry &registry, ecs::Entity entity);
bool CanPlayCard(const ecs::Registry &registry, ecs::Entity card, ecs::Entity target);

void PushEvent(ecs::Registry &registry, BattleEventType type, ecs::Entity source, ecs::Entity target, int value);
void DealDamage(ecs::Registry &registry, ecs::Entity source, ecs::Entity target, int amount);
void GainBlock(ecs::Registry &registry, ecs::Entity source, ecs::Entity target, int amount);

void UpdateBattleResult(ecs::Registry &registry);
