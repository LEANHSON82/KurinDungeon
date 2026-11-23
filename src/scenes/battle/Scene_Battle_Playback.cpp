#include "scenes/battle/Scene_Battle.h"
#include "engine/GameEngine.h"
#include "ui/Style.h"

static constexpr int eventGapFrames = 21;
static constexpr int floatingTextFrames = 54;
static constexpr float floatingTextRise = 1.0f; // pixels per frame
static constexpr float textHeight = 110.0f;

bool Scene_Battle::isPlayingBack() const
{
    return !m_events.empty() || m_playbackTimer > 0;
}

void Scene_Battle::setAnimation(const std::shared_ptr<Entity> &entity, const std::string &name, bool repeat)
{
    if (!entity || !entity->has<CActor>() || entity->get<CActor>().fallen)
    {
        return;
    }
    const Assets &assets = m_game->assets();
    std::string animation = entity->get<CActor>().sprites + name;
    if (!assets.hasAnimation(animation))
    {
        animation = entity->get<CActor>().sprites + "Idle";
        repeat = true;
    }
    entity->add<CAnimation>(assets.getAnimation(animation), repeat);
}

void Scene_Battle::say(const std::shared_ptr<Entity> &entity, const std::string &text, Color color)
{
    Vector2 feet = entity->get<CTransform>().pos;
    std::shared_ptr<Entity> label = m_entityManager.addEntity("text");
    label->add<CFloatingText>(text, color, floatingTextFrames);
    label->add<CTransform>(Vector2{feet.x, feet.y - textHeight});
}

void Scene_Battle::playEvent(const BattleEvent &event)
{
    const Assets &assets = m_game->assets();
    switch (event.type)
    {
    case BattleEventType::Damage:
        setAnimation(event.source, "Attack", false);
        setAnimation(event.target, "Hurt", false);
        if (event.value > 0)
        {
            say(event.target, "-" + std::to_string(event.value), redColor);
            assets.playSound("Hit");
        }
        else
        {
            say(event.target, "Blocked", tealColor);
            assets.playSound("Block");
        }
        break;
    case BattleEventType::Block:
        setAnimation(event.source, "Block", false);
        say(event.target, "+" + std::to_string(event.value) + " Block", tealColor);
        assets.playSound("Block");
        break;
    case BattleEventType::HpLoss:
        setAnimation(event.target, "Hurt", false);
        say(event.target, "-" + std::to_string(event.value), redColor);
        assets.playSound("Hit");
        break;
    case BattleEventType::Status:
    {
        const StatusDefinition &status = GetStatusDefinition(event.status);
        setAnimation(event.source, "Attack", false);
        say(event.target, "+" + std::to_string(event.value) + " " + status.name, status.harmful ? debuffColor : goldColor);
        break;
    }
    case BattleEventType::Death:
        setAnimation(event.target, "Death", false);
        event.target->get<CActor>().fallen = true;
        break;
    }
}

void Scene_Battle::sPlayback()
{
    if (m_playbackTimer > 0)
    {
        m_playbackTimer--;
    }
    if (m_playbackTimer == 0 && !m_events.empty())
    {
        BattleEvent event = m_events.front();
        m_events.pop_front();
        playEvent(event);
        m_playbackTimer = eventGapFrames;
    }

    if (!isPlayingBack() && m_result != BattleResult::InProgress && !m_endSoundPlayed)
    {
        m_game->assets().playSound(m_result == BattleResult::Won ? "Victory" : "Defeat");
        m_endSoundPlayed = true;
    }
}

void Scene_Battle::sAnimation()
{
    for (const std::shared_ptr<Entity> &entity : m_entityManager.getEntities())
    {
        if (!entity->has<CAnimation>())
        {
            continue;
        }
        CAnimation &animation = entity->get<CAnimation>();
        animation.animation.update();
        // A one-shot animation returns to idle, except death, which stays on its last frame.
        if (!animation.repeat && animation.animation.hasEnded() && !entity->get<CActor>().fallen)
        {
            setAnimation(entity, "Idle", true);
        }
    }
}

void Scene_Battle::sFloatingText()
{
    for (const std::shared_ptr<Entity> &label : m_entityManager.getEntities("text"))
    {
        CFloatingText &text = label->get<CFloatingText>();
        label->get<CTransform>().pos.y -= floatingTextRise;
        if (--text.framesLeft <= 0)
        {
            label->destroy();
        }
    }
}
