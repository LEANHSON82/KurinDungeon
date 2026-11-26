#include <algorithm>
#include "battlePlayback.h"
#include "battleLayout.h"
#include "screen/ui/style.h"

const float frameTime = 0.1f;
const float eventGap = 0.35f;
const float textLifetime = 0.9f;
const float textRiseSpeed = 60.0f;
const float textHeight = 110.0f;

static ActorView &GetActor(BattleScreenState &state, int actor)
{
    return actor == playerActor ? state.player : state.enemies[actor];
}

const CharacterSprites &GetActorSprites(const Battle &battle, const Assets &assets, int actor)
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
    if (actor.animation == AnimationType::Idle || actor.dead)
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
    state.texts.push_back({text, {feet.x, feet.y - textHeight}, color, 0.0f});
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
            AddText(state, battle, event.target, TextFormat("-%d", event.value), redColor);
            PlayGameSound(assets.hit);
        }
        else
        {
            AddText(state, battle, event.target, "Blocked", tealColor);
            PlayGameSound(assets.block);
        }
        break;
    case BattleEventType::Block:
        StartAnimation(GetActor(state, event.source), AnimationType::Block);
        AddText(state, battle, event.target, TextFormat("+%d Block", event.value), tealColor);
        PlayGameSound(assets.block);
        break;
    case BattleEventType::HpLoss:
        StartAnimation(GetActor(state, event.target), AnimationType::Hurt);
        AddText(state, battle, event.target, TextFormat("-%d", event.value), redColor);
        PlayGameSound(assets.hit);
        break;
    case BattleEventType::Status:
    {
        const StatusDefinition &status = GetStatusDefinition(event.status);
        StartAnimation(GetActor(state, event.source), AnimationType::Attack);
        AddText(state, battle, event.target, TextFormat("+%d %s", event.value, status.name), status.harmful ? debuffColor : goldColor);
        break;
    }
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

    AdvanceActor(state.player, GetActorSprites(battle, assets, playerActor), deltaTime);
    for (int i = 0; i < static_cast<int>(state.enemies.size()); i++)
    {
        AdvanceActor(state.enemies[i], GetActorSprites(battle, assets, i), deltaTime);
    }

    for (FloatingText &text : state.texts)
    {
        text.time += deltaTime;
        text.position.y -= textRiseSpeed * deltaTime;
    }
    if (!state.texts.empty() && state.texts.front().time > textLifetime)
    {
        state.texts.erase(state.texts.begin());
    }

    state.eventTimer = std::max(0.0f, state.eventTimer - deltaTime);
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
    if (sheet.frameCount <= 0)
    {
        return 0;
    }
    int frame = static_cast<int>(actor.time / frameTime);
    if (actor.animation == AnimationType::Idle)
    {
        return frame % sheet.frameCount;
    }
    return std::min(frame, sheet.frameCount - 1);
}
