#pragma once
#include <raylib.h>
#include "combat/battle.h"

Vector2 GetActorFeet(int actor, int enemyCount);
Vector2 GetActorInfo(int actor, int enemyCount);
Vector2 GetActorIntent(int actor, int enemyCount);
Rectangle GetActorRect(int actor, int enemyCount);

Rectangle GetHudRect();
Vector2 GetHealthCenter();
Vector2 GetEnergyCenter();
Rectangle GetCardRect(int index, int handCount, bool raised);

Rectangle GetEndTurnButton();
Rectangle GetContinueButton();

int GetCardUnderMouse(const Battle &battle, int selectedCard);
int GetEnemyUnderMouse(const Battle &battle);
