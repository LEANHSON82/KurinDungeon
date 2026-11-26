#pragma once
#include "battleScreen.h"

void UpdatePlayback(BattleScreenState &state, Battle &battle, const Assets &assets, float deltaTime);
bool IsPlayingBack(const BattleScreenState &state);

const CharacterSprites &GetActorSprites(const Battle &battle, const Assets &assets, int actor);
int GetActorFrame(const ActorView &actor, const SpriteSheet &sheet);
