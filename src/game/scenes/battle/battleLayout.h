#pragma once
#include <vector>
#include <raylib.h>
#include "core/data/cards.h"
#include "ecs/registry.h"

Vector2 GetPlayerFeet();
Vector2 GetEnemyFeet(int slot, int enemyCount);
Vector2 GetInfoPoint(Vector2 feet);
Vector2 GetIntentPoint(Vector2 feet);
Rectangle GetActorRect(Vector2 feet);

Rectangle GetHudRect();
Vector2 GetHealthCenter();
Vector2 GetEnergyCenter();
Rectangle GetCardRect(int index, int handCount, bool raised);

Rectangle GetEndTurnButton();
Rectangle GetContinueButton();

const Card *GetSelectedCard(const ecs::Registry &registry);
ecs::Entity GetCardUnderMouse(const std::vector<ecs::Entity> &hand, ecs::Entity selectedCard);
ecs::Entity GetEnemyUnderMouse(const ecs::Registry &registry);
