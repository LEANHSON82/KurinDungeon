#include "core/systems/battle/deckSystem.h"
#include <algorithm>
#include "core/components/cards.h"
#include "core/state/battle.h"
#include "core/systems/battle/combatRules.h"

static void MoveToZone(ecs::Registry &registry, ecs::Entity card, Zone zone)
{
    InZone &inZone = registry.Get<InZone>(card);
    inZone.zone = zone;
    inZone.order = registry.Resource<BattleState>().nextOrder++;
}

void ShuffleDrawPile(ecs::Registry &registry)
{
    std::vector<ecs::Entity> pile = GetPile(registry, Zone::Draw);
    std::shuffle(pile.begin(), pile.end(), registry.Resource<BattleState>().rng);
    for (int i = 0; i < static_cast<int>(pile.size()); i++)
    {
        registry.Get<InZone>(pile[i]).order = i;
    }
}

void DrawCards(ecs::Registry &registry, int count)
{
    for (int i = 0; i < count; i++)
    {
        std::vector<ecs::Entity> drawPile = GetPile(registry, Zone::Draw);
        if (drawPile.empty())
        {
            std::vector<ecs::Entity> discardPile = GetPile(registry, Zone::Discard);
            if (discardPile.empty())
            {
                return;
            }
            for (ecs::Entity card : discardPile)
            {
                registry.Get<InZone>(card).zone = Zone::Draw;
            }
            ShuffleDrawPile(registry);
            drawPile = GetPile(registry, Zone::Draw);
        }

        MoveToZone(registry, drawPile.back(), Zone::Hand);
    }
}

void DiscardCard(ecs::Registry &registry, ecs::Entity card)
{
    MoveToZone(registry, card, Zone::Discard);
}

void DiscardHand(ecs::Registry &registry)
{
    for (ecs::Entity card : GetPile(registry, Zone::Hand))
    {
        MoveToZone(registry, card, Zone::Discard);
    }
}
