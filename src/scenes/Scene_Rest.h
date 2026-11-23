#pragma once
#include "engine/Animation.h"
#include "engine/Scene.h"

class Scene_Rest : public Scene
{
    Animation m_heroIdle;
    void rest();

public:
    explicit Scene_Rest(GameEngine *game);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
