#pragma once
#include "engine/Scene.h"

class Scene_End : public Scene
{
    bool m_won = false;

public:
    Scene_End(GameEngine *game, bool won);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
