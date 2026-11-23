#pragma once
#include "engine/Animation.h"
#include "engine/Scene.h"

class Scene_CharacterSelect : public Scene
{
    Animation m_heroIdle;
    void confirm();

public:
    explicit Scene_CharacterSelect(GameEngine *game);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
