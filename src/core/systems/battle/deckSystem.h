#pragma once
#include "ecs/registry.h"

void ShuffleDrawPile(ecs::Registry &registry);
void DrawCards(ecs::Registry &registry, int count);
void DiscardCard(ecs::Registry &registry, ecs::Entity card);
void DiscardHand(ecs::Registry &registry);
