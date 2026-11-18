#include "game/systems/floatingTextSystem.h"
#include <vector>
#include "game/components/visual.h"

static constexpr float textLifetime = 0.9f;
static constexpr float riseSpeed = 60.0f;

void FloatingTextSystem(ecs::Registry &registry, float deltaTime)
{
    std::vector<ecs::Entity> expired;
    registry.View<FloatingText, Position>().Each([&](ecs::Entity entity, FloatingText &text, Position &position) {
        text.age += deltaTime;
        position.value.y -= riseSpeed * deltaTime;
        if (text.age > textLifetime)
        {
            expired.push_back(entity);
        }
    });
    for (ecs::Entity entity : expired)
    {
        registry.Destroy(entity);
    }
}
