#pragma once
#include <random>
#include "event.h"

const Event &GetEvent(EventId id);

EventId RollEvent(std::mt19937 &rng);
