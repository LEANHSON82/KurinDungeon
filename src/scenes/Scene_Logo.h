#pragma once
#include "engine/Scene.h"

class Scene_Logo : public Scene
{
    void goToMenu();

public:
    explicit Scene_Logo(GameEngine *game);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
