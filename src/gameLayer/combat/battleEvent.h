#pragma once

const int playerActor = -1;

enum class BattleEventType
{
    Damage,
    Block,
    Buff,
    Death,
};

struct BattleEvent
{
    BattleEventType type = BattleEventType::Damage;
    int source = playerActor;
    int target = playerActor;
    int value = 0;
};
