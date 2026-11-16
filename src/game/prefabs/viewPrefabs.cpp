#include "game/prefabs/viewPrefabs.h"
#include <string>
#include "core/components/combat.h"
#include "game/components/visual.h"

void AttachActorVisuals(ecs::Registry &registry, ecs::Entity actor, const CharacterSprites &sprites, Vector2 feet, bool flip)
{
    registry.Emplace<Position>(actor, feet);
    registry.Emplace<Sprite>(actor, &sprites, flip);
    registry.Emplace<Animator>(actor);
}

ecs::Entity SpawnFloatingText(ecs::Registry &registry, Vector2 position, const char *text, Color color)
{
    ecs::Entity entity = registry.Create();
    registry.Emplace<FloatingText>(entity, std::string(text), color);
    registry.Emplace<Position>(entity, position);
    registry.Emplace<BattleScoped>(entity);
    return entity;
}
