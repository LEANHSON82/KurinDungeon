#pragma once
#include "ecs/registry.h"

enum class Scene
{
    Logo,
    Menu,
    CharacterSelect,
    Map,
    Battle,
    Reward,
    Rest,
    Victory,
    GameOver,
};

struct SceneState
{
    Scene current = Scene::Logo;
    Scene next = Scene::Logo;
    float time = 0.0f;
    bool quit = false;
};

inline void RequestScene(ecs::Registry &registry, Scene scene)
{
    registry.Resource<SceneState>().next = scene;
}
