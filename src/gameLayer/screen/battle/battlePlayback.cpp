#include <algorithm>
#include "battlePlayback.h"
#include "battleLayout.h"

static const float frameTime = 0.1f;
static const float eventGap = 0.35f;
static const float textLifetime = 0.9f;

static const Color damageColor{230, 69, 57, 255};
static const Color blockColor{146, 232, 192, 255};
static const Color buffColor{255, 238, 131, 255};

static ActorView &GetActor(BattleScreenState &state, int actor)
{
    return actor == playerActor ? state.player : state.enemies[actor];
}

static const CharacterSprites &GetSprites(const Battle &battle, const Assets &assets, int actor)
{
    return actor == playerActor ? assets.knight : GetEnemySprites(assets, battle.GetEnemies()[actor].id);
}

static void StartAnimation(ActorView &actor, AnimationType type)
{
    if (actor.dead)
    {
        return;
    }
    actor.animation = type;
    actor.time = 0.0f;
}

static void AdvanceActor(ActorView &actor, const CharacterSprites &sprites, float deltaTime)
{
    actor.time += deltaTime;
    if (actor.animation == AnimationType::Idle || actor.animation == AnimationType::Death)
    {
        return;
    }
    if (actor.time >= GetAnimation(sprites, actor.animation).frameCount * frameTime)
    {
        StartAnimation(actor, AnimationType::Idle);
    }
}

static void AddText(BattleScreenState &state, const Battle &battle, int actor, const char *text, Color color)
{
    Vector2 feet = GetActorFeet(actor, static_cast<int>(battle.GetEnemies().size()));
    state.texts.push_back({text, {feet.x, feet.y - 110.0f}, color, 0.0f});
}

static void PlayEvent(BattleScreenState &state, const Battle &battle, const Assets &assets, const BattleEvent &event)
{
    switch (event.type)
    {
    case BattleEventType::Damage:
        StartAnimation(GetActor(state, event.source), AnimationType::Attack);
        StartAnimation(GetActor(state, event.target), AnimationType::Hurt);
        if (event.value > 0)
        {
            AddText(state, battle, event.target, TextFormat("-%d", event.value), damageColor);
            PlayGameSound(assets.hit);
        }
        else
        {
            AddText(state, battle, event.target, "Blocked", blockColor);
            PlayGameSound(assets.block);
        }
        break;
    case BattleEventType::Block:
        StartAnimation(GetActor(state, event.source), AnimationType::Block);
        AddText(state, battle, event.target, TextFormat("+%d Block", event.value), blockColor);
        PlayGameSound(assets.block);
        break;
    case BattleEventType::Buff:
        StartAnimation(GetActor(state, event.source), AnimationType::Attack);
        AddText(state, battle, event.target, TextFormat("+%d Strength", event.value), buffColor);
        break;
    case BattleEventType::Death:
        StartAnimation(GetActor(state, event.target), AnimationType::Death);
        GetActor(state, event.target).dead = true;
        break;
    }
}

void UpdatePlayback(BattleScreenState &state, Battle &battle, const Assets &assets, float deltaTime)
{
    for (const BattleEvent &event : battle.TakeEvents())
    {
        state.pending.push_back(event);
    }

    AdvanceActor(state.player, assets.knight, deltaTime);
    for (int i = 0; i < static_cast<int>(state.enemies.size()); i++)
    {
        AdvanceActor(state.enemies[i], GetSprites(battle, assets, i), deltaTime);
    }

    for (FloatingText &text : state.texts)
    {
        text.time += deltaTime;
        text.position.y -= 60.0f * deltaTime;
    }
    if (!state.texts.empty() && state.texts.front().time > textLifetime)
    {
        state.texts.erase(state.texts.begin());
    }

    state.eventTimer -= deltaTime;
    if (state.eventTimer <= 0.0f && !state.pending.empty())
    {
        PlayEvent(state, battle, assets, state.pending.front());
        state.pending.erase(state.pending.begin());
        state.eventTimer = eventGap;
    }

    if (!IsPlayingBack(state) && battle.GetResult() != BattleResult::InProgress && !state.endSoundPlayed)
    {
        PlayGameSound(battle.GetResult() == BattleResult::Won ? assets.victory : assets.defeat);
        state.endSoundPlayed = true;
    }
}

bool IsPlayingBack(const BattleScreenState &state)
{
    return !state.pending.empty() || state.eventTimer > 0.0f;
}

int GetActorFrame(const ActorView &actor, const SpriteSheet &sheet)
{
    int frame = static_cast<int>(actor.time / frameTime);
    if (actor.animation == AnimationType::Idle)
    {
        return frame % sheet.frameCount;
    }
    return std::min(frame, sheet.frameCount - 1);
}
