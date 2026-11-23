#pragma once
#include "engine/Scene.h"

class Scene_Menu : public Scene
{
public:
    explicit Scene_Menu(GameEngine *game);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
