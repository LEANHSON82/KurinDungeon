#pragma once
#include "ecs/registry.h"
#include "game/components/visual.h"

const float animationFrameTime = 0.1f;

void StartAnimation(Animator &animator, AnimationType type);

void AnimationSystem(ecs::Registry &registry, float deltaTime);

int GetAnimationFrame(const Animator &animator, const SpriteSheet &sheet);
