#pragma once
#include <vector>
#include "core/components/map.h"
#include "core/data/cards.h"
#include "core/data/enemies.h"
#include "ecs/registry.h"

ecs::Entity SpawnPlayer(ecs::Registry &registry);

ecs::Entity SpawnDeckCard(ecs::Registry &registry, CardId id);

void SpawnStartingDeck(ecs::Registry &registry);

ecs::Entity SpawnEnemy(ecs::Registry &registry, EnemyId id, int slot);

ecs::Entity SpawnRoom(ecs::Registry &registry, RoomType type, int floor, int column);
