#pragma once
#include "battleScreen.h"

void UpdatePlayback(BattleScreenState &state, Battle &battle, const Assets &assets, float deltaTime);
bool IsPlayingBack(const BattleScreenState &state);

int GetActorFrame(const ActorView &actor, const SpriteSheet &sheet);
