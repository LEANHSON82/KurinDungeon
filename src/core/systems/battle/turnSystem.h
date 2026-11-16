#pragma once
#include "ecs/registry.h"

void ChooseEnemyIntents(ecs::Registry &registry);
void StartPlayerTurn(ecs::Registry &registry);
void EndPlayerTurn(ecs::Registry &registry);
