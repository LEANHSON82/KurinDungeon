#pragma once
#include "ecs/registry.h"

void PlaybackSystem(ecs::Registry &registry, float deltaTime);

bool IsPlayingBack(const ecs::Registry &registry);
