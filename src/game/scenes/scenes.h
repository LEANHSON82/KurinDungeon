#pragma once
#include "ecs/registry.h"

void UpdateLogoScene(ecs::Registry &registry);
void DrawLogoScene(const ecs::Registry &registry);

void UpdateMenuScene(ecs::Registry &registry);
void DrawMenuScene(const ecs::Registry &registry);

void UpdateCharacterSelectScene(ecs::Registry &registry);
void DrawCharacterSelectScene(const ecs::Registry &registry);

void LayoutMapScene(ecs::Registry &registry);
void UpdateMapScene(ecs::Registry &registry);
void DrawMapScene(const ecs::Registry &registry);

void SetupBattleScene(ecs::Registry &registry);
void TeardownBattleScene(ecs::Registry &registry);
void UpdateBattleScene(ecs::Registry &registry, float deltaTime);
void BattleInputSystem(ecs::Registry &registry);
void DrawBattleScene(const ecs::Registry &registry);

void UpdateRewardScene(ecs::Registry &registry);
void DrawRewardScene(const ecs::Registry &registry);

void UpdateRestScene(ecs::Registry &registry);
void DrawRestScene(const ecs::Registry &registry);

void UpdateEndScene(ecs::Registry &registry);
void DrawEndScene(const ecs::Registry &registry);
