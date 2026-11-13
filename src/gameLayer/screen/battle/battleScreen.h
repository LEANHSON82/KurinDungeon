#pragma once
#include <string>
#include <vector>
#include "assets/assets.h"
#include "combat/battle.h"
#include "screen/screenAction.h"

struct ActorView
{
    AnimationType animation = AnimationType::Idle;
    float time = 0.0f;
    bool dead = false;
};

struct FloatingText
{
    std::string text;
    Vector2 position{};
    Color color = WHITE;
    float time = 0.0f;
};

struct BattleScreenState
{
    int selectedCard = -1;
    int target = 0;

    std::vector<BattleEvent> pending;
    float eventTimer = 0.0f;
    bool endSoundPlayed = false;
    ActorView player;
    std::vector<ActorView> enemies;
    std::vector<FloatingText> texts;
};

ScreenAction UpdateBattleScreen(BattleScreenState &state, Battle &battle, const Assets &assets, float deltaTime);

void DrawBattleScreen(const BattleScreenState &state, const Battle &battle, const Assets &assets);
