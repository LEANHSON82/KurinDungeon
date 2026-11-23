#pragma once
#include <string>
#include <raylib.h>

// An input event sent to the current scene: a name such as "END_TURN" and a type
// ("START" when the key or button goes down, "END" when it goes up).
class Action
{
    std::string m_name = "NONE";
    std::string m_type = "NONE";
    Vector2 m_pos{};

public:
    Action() = default;
    Action(const std::string &name, const std::string &type) : m_name(name), m_type(type) {}
    Action(const std::string &name, const std::string &type, Vector2 pos) : m_name(name), m_type(type), m_pos(pos) {}

    const std::string &name() const { return m_name; }
    const std::string &type() const { return m_type; }
    Vector2 pos() const { return m_pos; }
};
