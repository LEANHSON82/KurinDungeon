#pragma once
#include "status.h"

const int playerActor = -1;

enum class BattleEventType
{
    Damage,
    Block,
    Status,
    HpLoss,
    Death,
};

struct BattleEvent
{
    BattleEventType type = BattleEventType::Damage;
    int source = playerActor;
    int target = playerActor;
    int value = 0;
    StatusType status = StatusType::Strength;
};
