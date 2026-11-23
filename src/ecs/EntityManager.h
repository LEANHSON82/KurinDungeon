#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ecs/Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    std::map<std::string, EntityVec> m_entityMap;
    std::size_t m_totalEntities = 0;

    void removeDeadEntities(EntityVec &vec);

public:
    // Adds waiting entities and removes destroyed ones. Call once per frame, never while
    // looping over getEntities(), so the lists do not change under the loop.
    void update();

    // The new entity shows up in getEntities() after the next update().
    std::shared_ptr<Entity> addEntity(const std::string &tag);

    const EntityVec &getEntities();
    const EntityVec &getEntities(const std::string &tag);
};
