#include "engine/Scene.h"

void Scene::registerAction(int key, const std::string &name)
{
    m_actionMap[key] = name;
}

void Scene::doAction(const Action &action)
{
    bool leftClick = action.name() == "LEFT_CLICK";
    if (leftClick && action.type() == "START")
    {
        m_pressPos = action.pos();
        m_pressed = true;
    }

    sDoAction(action);

    if (leftClick && action.type() == "END")
    {
        m_pressed = false;
    }
}

bool Scene::isClicked(const Action &action, Rectangle button) const
{
    return action.name() == "LEFT_CLICK" && action.type() == "END" && m_pressed &&
           CheckCollisionPointRec(m_pressPos, button) && CheckCollisionPointRec(action.pos(), button);
}

bool Scene::isHeld(Rectangle button) const
{
    return m_pressed && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(m_pressPos, button);
}
