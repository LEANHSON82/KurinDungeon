#pragma once
#include <cassert>
#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "ecs/entity.h"
#include "ecs/pool.h"

namespace ecs
{
namespace detail
{
inline std::size_t NextTypeId()
{
    static std::size_t next = 0;
    return next++;
}

template <class T>
std::size_t TypeId()
{
    static_assert(std::is_same_v<T, std::remove_cv_t<T>>, "Use the plain component type, without const");
    static const std::size_t id = NextTypeId();
    return id;
}
}

template <bool IsConst, class... Ts>
class BasicView
{
    static_assert(sizeof...(Ts) > 0, "A view needs at least one component type");

    template <class T>
    using PoolPtr = std::conditional_t<IsConst, const Pool<T> *, Pool<T> *>;

public:
    explicit BasicView(PoolPtr<Ts>... sources) : pools(sources...) {}

    std::vector<Entity> Collect() const
    {
        std::vector<Entity> result;
        const SparseSet *smallest = Smallest();
        if (smallest == nullptr)
        {
            return result;
        }
        for (Entity entity : smallest->Entities())
        {
            if (Contains(entity))
            {
                result.push_back(entity);
            }
        }
        return result;
    }

    // The callback may destroy or change the current entity. It must not remove
    // components of OTHER entities that are in this view: swap-and-pop moves
    // storage and the references passed to the callback would dangle. Collect
    // such entities and destroy them after Each returns.
    template <class F>
    void Each(F &&function) const
    {
        for (Entity entity : Collect())
        {
            if (Contains(entity))
            {
                function(entity, std::get<PoolPtr<Ts>>(pools)->Get(entity)...);
            }
        }
    }

private:
    bool Contains(Entity entity) const
    {
        return ((std::get<PoolPtr<Ts>>(pools) != nullptr && std::get<PoolPtr<Ts>>(pools)->Has(entity)) && ...);
    }

    const SparseSet *Smallest() const
    {
        const SparseSet *smallest = nullptr;
        bool missing = false;
        auto consider = [&](const SparseSet *pool) {
            if (pool == nullptr)
            {
                missing = true;
                return;
            }
            if (smallest == nullptr || pool->Size() < smallest->Size())
            {
                smallest = pool;
            }
        };
        (consider(std::get<PoolPtr<Ts>>(pools)), ...);
        return missing ? nullptr : smallest;
    }

    std::tuple<PoolPtr<Ts>...> pools;
};

class Registry
{
public:
    Registry() = default;
    Registry(const Registry &) = delete;
    Registry &operator=(const Registry &) = delete;

    Entity Create()
    {
        if (!freeList.empty())
        {
            std::uint32_t index = freeList.back();
            freeList.pop_back();
            alive[index] = true;
            return {index, versions[index]};
        }
        std::uint32_t index = static_cast<std::uint32_t>(versions.size());
        versions.push_back(0);
        alive.push_back(true);
        return {index, 0};
    }

    void Destroy(Entity entity)
    {
        if (!Valid(entity))
        {
            return;
        }
        for (std::unique_ptr<SparseSet> &pool : pools)
        {
            if (pool)
            {
                pool->Remove(entity);
            }
        }
        versions[entity.index]++;
        alive[entity.index] = false;
        freeList.push_back(entity.index);
    }

    bool Valid(Entity entity) const
    {
        return entity.index < versions.size() && alive[entity.index] && versions[entity.index] == entity.version;
    }

    void Clear()
    {
        for (std::uint32_t index = 0; index < versions.size(); index++)
        {
            if (alive[index])
            {
                Destroy({index, versions[index]});
            }
        }
    }

    template <class T, class... Args>
    T &Emplace(Entity entity, Args &&...args)
    {
        assert(Valid(entity));
        return Assure<T>().Emplace(entity, std::forward<Args>(args)...);
    }

    template <class T, class... Args>
    T &EmplaceOrReplace(Entity entity, Args &&...args)
    {
        assert(Valid(entity));
        return Assure<T>().EmplaceOrReplace(entity, std::forward<Args>(args)...);
    }

    template <class T>
    void Remove(Entity entity)
    {
        if (Pool<T> *pool = Find<T>())
        {
            pool->Remove(entity);
        }
    }

    template <class T>
    void RemoveAll()
    {
        if (Pool<T> *pool = Find<T>())
        {
            pool->Clear();
        }
    }

    template <class T>
    bool Has(Entity entity) const
    {
        const Pool<T> *pool = Find<T>();
        return pool != nullptr && pool->Has(entity);
    }

    template <class T>
    T &Get(Entity entity)
    {
        Pool<T> *pool = Find<T>();
        assert(pool != nullptr);
        return pool->Get(entity);
    }

    template <class T>
    const T &Get(Entity entity) const
    {
        const Pool<T> *pool = Find<T>();
        assert(pool != nullptr);
        return pool->Get(entity);
    }

    template <class T>
    T *TryGet(Entity entity)
    {
        Pool<T> *pool = Find<T>();
        return pool != nullptr && pool->Has(entity) ? &pool->Get(entity) : nullptr;
    }

    template <class T>
    const T *TryGet(Entity entity) const
    {
        const Pool<T> *pool = Find<T>();
        return pool != nullptr && pool->Has(entity) ? &pool->Get(entity) : nullptr;
    }

    template <class... Ts>
    BasicView<false, Ts...> View()
    {
        return BasicView<false, Ts...>(&Assure<Ts>()...);
    }

    template <class... Ts>
    BasicView<true, Ts...> View() const
    {
        return BasicView<true, Ts...>(Find<Ts>()...);
    }

    template <class T, class... Args>
    T &SetResource(Args &&...args)
    {
        std::size_t id = detail::TypeId<T>();
        if (id >= resources.size())
        {
            resources.resize(id + 1);
        }
        std::shared_ptr<T> resource(new T{std::forward<Args>(args)...});
        T &reference = *resource;
        resources[id] = std::move(resource);
        return reference;
    }

    template <class T>
    T &Resource()
    {
        std::size_t id = detail::TypeId<T>();
        assert(id < resources.size() && resources[id]);
        return *static_cast<T *>(resources[id].get());
    }

    template <class T>
    const T &Resource() const
    {
        std::size_t id = detail::TypeId<T>();
        assert(id < resources.size() && resources[id]);
        return *static_cast<const T *>(resources[id].get());
    }

private:
    template <class T>
    Pool<T> &Assure()
    {
        std::size_t id = detail::TypeId<T>();
        if (id >= pools.size())
        {
            pools.resize(id + 1);
        }
        if (!pools[id])
        {
            pools[id] = std::make_unique<Pool<T>>();
        }
        return static_cast<Pool<T> &>(*pools[id]);
    }

    template <class T>
    Pool<T> *Find()
    {
        std::size_t id = detail::TypeId<T>();
        return id < pools.size() ? static_cast<Pool<T> *>(pools[id].get()) : nullptr;
    }

    template <class T>
    const Pool<T> *Find() const
    {
        std::size_t id = detail::TypeId<T>();
        return id < pools.size() ? static_cast<const Pool<T> *>(pools[id].get()) : nullptr;
    }

    std::vector<std::uint32_t> versions;
    std::vector<bool> alive;
    std::vector<std::uint32_t> freeList;
    std::vector<std::unique_ptr<SparseSet>> pools;
    std::vector<std::shared_ptr<void>> resources;
};
}
