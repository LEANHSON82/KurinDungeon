#pragma once
#include <vector>
#include "data/Cards.h"
#include "engine/Scene.h"

class Scene_Reward : public Scene
{
    std::vector<CardId> m_choices;
    void choose(int index);

public:
    explicit Scene_Reward(GameEngine *game);
    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;
};
