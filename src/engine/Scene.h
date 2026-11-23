#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <raylib.h>
#include "ecs/EntityManager.h"
#include "engine/Action.h"

class GameEngine;

using ActionMap = std::map<int, std::string>; // raylib key -> action name

class Scene
{
protected:
    GameEngine *m_game = nullptr;
    EntityManager m_entityManager;
    ActionMap m_actionMap;
    std::size_t m_currentFrame = 0;

    // Where the left mouse button went down, so a button only counts a click that
    // starts and ends on it.
    Vector2 m_pressPos{};
    bool m_pressed = false;

    bool isClicked(const Action &action, Rectangle button) const;
    bool isHeld(Rectangle button) const;

public:
    explicit Scene(GameEngine *game) : m_game(game) {}
    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void sDoAction(const Action &action) = 0;
    virtual void sRender() = 0;

    // Called by GameEngine for every input event.
    void doAction(const Action &action);
    void registerAction(int key, const std::string &name);

    const ActionMap &getActionMap() const { return m_actionMap; }
};
