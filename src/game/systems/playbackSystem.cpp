#include "game/systems/playbackSystem.h"
#include "core/state/battle.h"
#include "game/assets/assets.h"
#include "game/components/visual.h"
#include "game/prefabs/viewPrefabs.h"
#include "game/state/battleView.h"
#include "game/systems/animationSystem.h"
#include "game/systems/floatingTextSystem.h"

static const float eventGap = 0.35f;
static const float textHeight = 110.0f;

static const Color damageColor{230, 69, 57, 255};
static const Color blockColor{146, 232, 192, 255};
static const Color buffColor{255, 238, 131, 255};

static void Animate(ecs::Registry &registry, ecs::Entity actor, AnimationType type)
{
    if (Animator *animator = registry.TryGet<Animator>(actor))
    {
        StartAnimation(*animator, type);
    }
}

static void Say(ecs::Registry &registry, ecs::Entity actor, const char *text, Color color)
{
    const Position *position = registry.TryGet<Position>(actor);
    if (position == nullptr)
    {
        return;
    }
    Vector2 above{position->value.x, position->value.y - textHeight};
    SpawnFloatingText(registry, above, text, color);
}

static void PlayEvent(ecs::Registry &registry, const BattleEvent &event)
{
    const Assets &assets = registry.Resource<Assets>();
    switch (event.type)
    {
    case BattleEventType::Damage:
        Animate(registry, event.source, AnimationType::Attack);
        Animate(registry, event.target, AnimationType::Hurt);
        if (event.value > 0)
        {
            Say(registry, event.target, TextFormat("-%d", event.value), damageColor);
            PlayGameSound(assets.hit);
        }
        else
        {
            Say(registry, event.target, "Blocked", blockColor);
            PlayGameSound(assets.block);
        }
        break;
    case BattleEventType::Block:
        Animate(registry, event.source, AnimationType::Block);
        Say(registry, event.target, TextFormat("+%d Block", event.value), blockColor);
        PlayGameSound(assets.block);
        break;
    case BattleEventType::Buff:
        Animate(registry, event.source, AnimationType::Attack);
        Say(registry, event.target, TextFormat("+%d Strength", event.value), buffColor);
        break;
    case BattleEventType::Death:
        Animate(registry, event.target, AnimationType::Death);
        if (Animator *animator = registry.TryGet<Animator>(event.target))
        {
            animator->dead = true;
        }
        break;
    }
}

void PlaybackSystem(ecs::Registry &registry, float deltaTime)
{
    AnimationSystem(registry, deltaTime);
    FloatingTextSystem(registry, deltaTime);

    Playback &playback = registry.Resource<Playback>();
    BattleEvents &events = registry.Resource<BattleEvents>();

    playback.timer -= deltaTime;
    if (playback.timer <= 0.0f && !events.pending.empty())
    {
        BattleEvent event = events.pending.front();
        events.pending.pop_front();
        PlayEvent(registry, event);
        playback.timer = eventGap;
    }

    BattleResult result = registry.Resource<BattleState>().result;
    if (!IsPlayingBack(registry) && result != BattleResult::InProgress && !playback.endSoundPlayed)
    {
        const Assets &assets = registry.Resource<Assets>();
        PlayGameSound(result == BattleResult::Won ? assets.victory : assets.defeat);
        playback.endSoundPlayed = true;
    }
}

bool IsPlayingBack(const ecs::Registry &registry)
{
    return !registry.Resource<BattleEvents>().pending.empty() || registry.Resource<Playback>().timer > 0.0f;
}
