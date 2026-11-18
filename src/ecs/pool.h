#pragma once
#include <cassert>
#include <cstdint>
#include <deque>
#include <limits>
#include <utility>
#include <vector>
#include "ecs/entity.h"

namespace ecs
{
class SparseSet
{
public:
    virtual ~SparseSet() = default;

    bool Has(Entity entity) const
    {
        return entity.index < sparse.size() && sparse[entity.index] != npos && dense[sparse[entity.index]] == entity;
    }

    std::size_t Size() const { return dense.size(); }
    const std::vector<Entity> &Entities() const { return dense; }

    virtual void Remove(Entity entity) = 0;
    virtual void Clear() = 0;

protected:
    static constexpr std::uint32_t npos = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t Slot(Entity entity) const { return sparse[entity.index]; }

    void Insert(Entity entity)
    {
        if (entity.index >= sparse.size())
        {
            sparse.resize(entity.index + 1, npos);
        }
        sparse[entity.index] = static_cast<std::uint32_t>(dense.size());
        dense.push_back(entity);
    }

    std::uint32_t Erase(Entity entity)
    {
        std::uint32_t slot = sparse[entity.index];
        Entity last = dense.back();
        dense[slot] = last;
        sparse[last.index] = slot;
        dense.pop_back();
        sparse[entity.index] = npos;
        return slot;
    }

    void ClearSet()
    {
        sparse.clear();
        dense.clear();
    }

private:
    std::vector<std::uint32_t> sparse;
    std::vector<Entity> dense;
};

template <class T>
class Pool final : public SparseSet
{
public:
    // Adds a component the entity must not have yet.
    template <class... Args>
    T &Emplace(Entity entity, Args &&...args)
    {
        assert(!Has(entity) && "Entity already has this component; use EmplaceOrReplace");
        Insert(entity);
        data.push_back(T{std::forward<Args>(args)...});
        return data.back();
    }

    template <class... Args>
    T &EmplaceOrReplace(Entity entity, Args &&...args)
    {
        if (Has(entity))
        {
            T &component = data[Slot(entity)];
            component = T{std::forward<Args>(args)...};
            return component;
        }
        return Emplace(entity, std::forward<Args>(args)...);
    }

    T &Get(Entity entity)
    {
        assert(Has(entity));
        return data[Slot(entity)];
    }

    const T &Get(Entity entity) const
    {
        assert(Has(entity));
        return data[Slot(entity)];
    }

    void Remove(Entity entity) override
    {
        if (!Has(entity))
        {
            return;
        }
        std::uint32_t slot = Erase(entity);
        if (slot != data.size() - 1)
        {
            data[slot] = std::move(data.back());
        }
        data.pop_back();
    }

    void Clear() override
    {
        ClearSet();
        data.clear();
    }

private:
    std::deque<T> data;
};
}
