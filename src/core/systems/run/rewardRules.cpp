#include "core/systems/run/rewardRules.h"

std::vector<CardId> RollRewards(std::mt19937 &rng, int count)
{
    std::vector<CardId> pool;
    for (int i = 0; i < cardCount; i++)
    {
        CardId id = static_cast<CardId>(i);
        if (GetCard(id).rewardable)
        {
            pool.push_back(id);
        }
    }

    std::vector<CardId> choices;
    while (static_cast<int>(choices.size()) < count && !pool.empty())
    {
        std::uniform_int_distribution<int> pick(0, static_cast<int>(pool.size()) - 1);
        int index = pick(rng);
        choices.push_back(pool[index]);
        pool.erase(pool.begin() + index);
    }

    return choices;
}
