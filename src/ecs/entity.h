#pragma once
#include <cstdint>
#include <limits>

namespace ecs
{
struct Entity
{
    std::uint32_t index = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t version = 0;

    bool IsNull() const { return index == std::numeric_limits<std::uint32_t>::max(); }

    friend bool operator==(Entity a, Entity b) { return a.index == b.index && a.version == b.version; }
    friend bool operator!=(Entity a, Entity b) { return !(a == b); }
};

inline constexpr Entity nullEntity{};
}
