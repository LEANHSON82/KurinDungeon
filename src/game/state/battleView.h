#pragma once
#include "ecs/entity.h"

struct BattleUi
{
    ecs::Entity selectedCard;
    ecs::Entity target;
};

struct Playback
{
    float timer = 0.0f;
    bool endSoundPlayed = false;
};
