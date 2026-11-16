#include "game/systems/animationSystem.h"
#include <algorithm>

void StartAnimation(Animator &animator, AnimationType type)
{
    if (animator.dead)
    {
        return;
    }
    animator.animation = type;
    animator.time = 0.0f;
}

void AnimationSystem(ecs::Registry &registry, float deltaTime)
{
    registry.View<Animator, Sprite>().Each([&](ecs::Entity, Animator &animator, const Sprite &sprite) {
        animator.time += deltaTime;
        if (animator.animation == AnimationType::Idle || animator.animation == AnimationType::Death)
        {
            return;
        }
        if (animator.time >= GetAnimation(*sprite.sprites, animator.animation).frameCount * animationFrameTime)
        {
            StartAnimation(animator, AnimationType::Idle);
        }
    });
}

int GetAnimationFrame(const Animator &animator, const SpriteSheet &sheet)
{
    int frame = static_cast<int>(animator.time / animationFrameTime);
    if (animator.animation == AnimationType::Idle)
    {
        return frame % sheet.frameCount;
    }
    return std::min(frame, sheet.frameCount - 1);
}
