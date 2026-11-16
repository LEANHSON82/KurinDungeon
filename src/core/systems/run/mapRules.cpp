#include "core/systems/run/mapRules.h"
#include <algorithm>
#include "core/components/map.h"

bool HasLink(const ecs::Registry &registry, ecs::Entity fromRoom, ecs::Entity toRoom)
{
    const RoomLinks *links = registry.TryGet<RoomLinks>(fromRoom);
    if (links == nullptr || toRoom.IsNull())
    {
        return false;
    }
    return std::find(links->next.begin(), links->next.end(), toRoom) != links->next.end();
}

bool CanVisit(const ecs::Registry &registry, ecs::Entity fromRoom, ecs::Entity toRoom)
{
    if (!registry.Has<Room>(fromRoom) || !registry.Has<Room>(toRoom))
    {
        return false;
    }
    if (!registry.Has<Cleared>(fromRoom) || registry.Has<Cleared>(toRoom))
    {
        return false;
    }
    return HasLink(registry, fromRoom, toRoom);
}
