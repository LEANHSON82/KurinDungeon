#include "core/prefabs/prefabs.h"
#include "core/components/cards.h"
#include "core/components/combat.h"
#include "core/state/run.h"

ecs::Entity SpawnPlayer(ecs::Registry &registry)
{
    ecs::Entity player = registry.Create();
    registry.Emplace<Player>(player);
    registry.Emplace<Health>(player, startingHp, startingHp);
    registry.Emplace<RunScoped>(player);
    return player;
}

ecs::Entity SpawnDeckCard(ecs::Registry &registry, CardId id)
{
    ecs::Entity card = registry.Create();
    registry.Emplace<DeckCard>(card, id);
    registry.Emplace<RunScoped>(card);
    return card;
}

void SpawnStartingDeck(ecs::Registry &registry)
{
    for (CardId id : GetStartingDeck())
    {
        SpawnDeckCard(registry, id);
    }
}

ecs::Entity SpawnEnemy(ecs::Registry &registry, EnemyId id, int slot)
{
    const EnemyDefinition &definition = GetEnemyDefinition(id);

    ecs::Entity enemy = registry.Create();
    registry.Emplace<Enemy>(enemy, id, slot);
    registry.Emplace<Health>(enemy, definition.maxHp, definition.maxHp);
    registry.Emplace<Block>(enemy);
    registry.Emplace<Strength>(enemy);
    registry.Emplace<Intent>(enemy);
    registry.Emplace<BattleScoped>(enemy);
    registry.Emplace<RunScoped>(enemy);
    if (definition.bossAbility)
    {
        registry.Emplace<BossAbility>(enemy);
    }
    return enemy;
}

ecs::Entity SpawnRoom(ecs::Registry &registry, RoomType type, int floor, int column)
{
    ecs::Entity room = registry.Create();
    registry.Emplace<Room>(room, type, floor, column);
    registry.Emplace<RoomLinks>(room);
    registry.Emplace<RunScoped>(room);
    return room;
}
