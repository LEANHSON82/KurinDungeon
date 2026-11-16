#pragma once
#include "ecs/registry.h"

bool HasLink(const ecs::Registry &registry, ecs::Entity fromRoom, ecs::Entity toRoom);

bool CanVisit(const ecs::Registry &registry, ecs::Entity fromRoom, ecs::Entity toRoom);
