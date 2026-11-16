#include "game/systems/floatingTextSystem.h"
#include "game/components/visual.h"

static const float textLifetime = 0.9f;
static const float riseSpeed = 60.0f;

void FloatingTextSystem(ecs::Registry &registry, float deltaTime)
{
    registry.View<FloatingText, Position>().Each([&](ecs::Entity entity, FloatingText &text, Position &position) {
        text.age += deltaTime;
        position.value.y -= riseSpeed * deltaTime;
        if (text.age > textLifetime)
        {
            registry.Destroy(entity);
        }
    });
}
