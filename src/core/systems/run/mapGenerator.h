#pragma once
#include <cstdint>
#include "ecs/registry.h"

ecs::Entity GenerateMap(ecs::Registry &registry, std::uint32_t seed);

bool ValidateMap(const ecs::Registry &registry);
