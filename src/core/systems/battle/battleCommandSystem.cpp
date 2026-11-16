#include "core/systems/battle/battleCommandSystem.h"
#include <string>
#include "core/components/cards.h"
#include "core/components/combat.h"
#include "core/state/battle.h"
#include "core/systems/battle/battleLog.h"
#include "core/systems/battle/cardPlaySystem.h"
#include "core/systems/battle/turnSystem.h"

static std::string DescribeCard(const ecs::Registry &registry, ecs::Entity card)
{
    const DeckCard *deckCard = registry.TryGet<DeckCard>(card);
    return deckCard != nullptr ? GetCard(deckCard->id).name : "none";
}

static std::string DescribeTarget(const ecs::Registry &registry, ecs::Entity target)
{
    const Enemy *enemy = registry.TryGet<Enemy>(target);
    if (enemy == nullptr)
    {
        return "none";
    }
    return std::string(GetEnemyDefinition(enemy->id).name) + " #" + std::to_string(enemy->slot);
}

void BattleCommandSystem(ecs::Registry &registry)
{
    std::vector<BattleCommand> queue;
    queue.swap(registry.Resource<BattleCommands>().queue);
    if (!registry.Resource<BattleState>().active)
    {
        return;
    }

    for (const BattleCommand &command : queue)
    {
        switch (command.type)
        {
        case BattleCommandType::PlayCard:
        {
            std::string label = "PlayCard(" + DescribeCard(registry, command.card) + " -> " + DescribeTarget(registry, command.target) + ")";
            bool played = PlayCard(registry, command.card, command.target);
            label += played ? " accepted" : " rejected";
            LogBattle(registry, label.c_str());
            break;
        }
        case BattleCommandType::EndTurn:
            LogBattle(registry, "Before EndTurn");
            EndPlayerTurn(registry);
            LogBattle(registry, "After EndTurn");
            break;
        }
    }
}
