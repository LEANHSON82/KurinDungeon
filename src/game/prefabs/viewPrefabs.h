#pragma once
#include <raylib.h>
#include "ecs/registry.h"
#include "game/assets/assets.h"

void AttachActorVisuals(ecs::Registry &registry, ecs::Entity actor, const CharacterSprites &sprites, Vector2 feet, bool flip);

ecs::Entity SpawnFloatingText(ecs::Registry &registry, Vector2 position, const char *text, Color color);
