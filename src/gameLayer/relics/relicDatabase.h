#pragma once
#include <random>
#include <vector>
#include "relic.h"

const Relic &GetRelic(RelicId id);

bool RollRelic(std::mt19937 &rng, const std::vector<RelicId> &owned, RelicId &relic);
