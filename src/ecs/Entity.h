#pragma once
#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include "ecs/Components.h"

using ComponentTuple = std::tuple<CTransform, CHealth, CBlock, CStrength, CIntent, CEnemy, CBossAbility, CCard,
                                  CAnimation, CActor, CFloatingText, CRoom>;

class Entity
{
    friend class EntityManager;

    bool m_active = true;
    std::size_t m_id = 0;
    std::string m_tag = "default";
    ComponentTuple m_components;

    // Only EntityManager creates entities.
    Entity(std::size_t id, const std::string &tag) : m_id(id), m_tag(tag) {}

public:
    // Marks the entity; EntityManager removes it on its next update().
    void destroy() { m_active = false; }

    std::size_t id() const { return m_id; }
    bool isActive() const { return m_active; }
    const std::string &tag() const { return m_tag; }

    template <typename T>
    bool has() const
    {
        return get<T>().has;
    }

    template <typename T, typename... TArgs>
    T &add(TArgs &&...args)
    {
        T &component = get<T>();
        component = T(std::forward<TArgs>(args)...);
        component.has = true;
        return component;
    }

    template <typename T>
    T &get()
    {
        return std::get<T>(m_components);
    }

    template <typename T>
    const T &get() const
    {
        return std::get<T>(m_components);
    }

    template <typename T>
    void remove()
    {
        get<T>() = T();
    }
};
