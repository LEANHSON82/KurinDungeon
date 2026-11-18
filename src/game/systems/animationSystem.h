#pragma once
#include "ecs/registry.h"
#include "game/components/visual.h"

inline constexpr float animationFrameTime = 0.1f;

void StartAnimation(Animator &animator, AnimationType type);

void AnimationSystem(ecs::Registry &registry, float deltaTime);

int GetAnimationFrame(const Animator &animator, const SpriteSheet &sheet);

// Frame of a looping animation driven by a clock, for sprites without an Animator.
int GetLoopFrame(const SpriteSheet &sheet, double time);
