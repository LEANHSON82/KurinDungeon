#include "ecs/EntityManager.h"
#include <algorithm>

void EntityManager::update()
{
    for (const std::shared_ptr<Entity> &entity : m_entitiesToAdd)
    {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }
    m_entitiesToAdd.clear();

    removeDeadEntities(m_entities);
    for (auto &[tag, entities] : m_entityMap)
    {
        removeDeadEntities(entities);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity> &entity) { return !entity->isActive(); }),
              vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}
