#pragma once
#include "ecs/registry.h"
#include "game/state/scene.h"

class Game
{
public:
    Game();
    ~Game();
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;

    void Update(float deltaTime);
    void Draw() const;
    bool ShouldQuit() const;

private:
    void UpdateScene(float deltaTime);
    void ApplySceneChange();
    void OnEnter(Scene scene);
    void OnExit(Scene scene);

    ecs::Registry registry;
};
