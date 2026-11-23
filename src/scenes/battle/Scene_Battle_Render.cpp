#include "scenes/battle/Scene_Battle.h"
#include "engine/GameEngine.h"
#include "scenes/battle/BattleLayout.h"
#include "ui/CardView.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static constexpr float barWidth = 216.0f;
static constexpr float statIconGap = 6.0f;
static constexpr float intentBannerWidth = 216.0f;
static constexpr float intentBannerHeight = 96.0f;
static constexpr float intentIconOffset = 28.0f;
static constexpr float intentValueOffset = 32.0f;
static constexpr float pileInset = 430.0f;
static constexpr float pileLabelOffset = 36.0f;
static constexpr float pileCountOffset = 24.0f;
static constexpr float exhaustLabelOffset = 82.0f;
static constexpr float tooltipGap = 12.0f;
static constexpr float floorShadeTop = 560.0f;
static constexpr float turnLabelY = 54.0f;
static constexpr float resultY = 400.0f;

const Card *Scene_Battle::selectedCard() const
{
    return m_selectedCard ? &GetCard(m_selectedCard->get<CCard>().id) : nullptr;
}

static Color CardColor(const Card &card)
{
    switch (card.kind)
    {
    case CardKind::Attack:
        return damageOutline;
    case CardKind::Skill:
        return blockOutline;
    case CardKind::Power:
        return goldColor;
    }
    return damageOutline;
}

Color Scene_Battle::outlineFor(const std::shared_ptr<Entity> &actor) const
{
    const Card *card = selectedCard();
    if (card == nullptr)
    {
        return outlineColor;
    }
    bool targeted = false;
    switch (card->target)
    {
    case Target::SingleEnemy:
        targeted = actor == m_target && isAlive(actor);
        break;
    case Target::AllEnemies:
        targeted = isAlive(actor);
        break;
    case Target::Self:
        targeted = actor == m_player;
        break;
    }
    return targeted ? CardColor(*card) : outlineColor;
}

void Scene_Battle::drawActors()
{
    auto draw = [this](const std::shared_ptr<Entity> &actor) {
        const CAnimation &animation = actor->get<CAnimation>();
        const CActor &data = actor->get<CActor>();
        std::size_t frame = animation.animation.frame(animation.repeat);
        Vector2 feet = actor->get<CTransform>().pos;
        if (!data.fallen)
        {
            DrawSpriteOutline(animation.animation, frame, feet, data.flip, outlineFor(actor));
        }
        DrawSprite(animation.animation, frame, feet, data.flip, WHITE);
    };
    draw(m_player);
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        draw(enemy);
    }
}

static void DrawStatIcon(const Assets &assets, const char *icon, Vector2 center, int value)
{
    DrawIcon(assets.getTexture(icon), center, uiScale, WHITE);
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", value), {center.x, center.y - smallTextNudge}, smallText, textColor);
}

static const char *IntentIcon(IntentType type)
{
    switch (type)
    {
    case IntentType::Attack:
        return "Sword";
    case IntentType::Defend:
        return "Shield";
    case IntentType::Buff:
        return "Star";
    }
    return "Sword";
}

static Vector2 InfoPoint(Vector2 feet)
{
    return {feet.x, feet.y + 46.0f};
}

void Scene_Battle::drawActorInfo()
{
    const Assets &assets = m_game->assets();
    int playerBlock = m_player->get<CBlock>().value;
    if (!m_player->get<CActor>().fallen && playerBlock > 0)
    {
        DrawStatIcon(assets, "Shield", InfoPoint(m_player->get<CTransform>().pos), playerBlock);
    }

    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        if (enemy->get<CActor>().fallen)
        {
            continue;
        }
        Vector2 feet = enemy->get<CTransform>().pos;
        Vector2 center = InfoPoint(feet);
        const CHealth &health = enemy->get<CHealth>();
        DrawHealthBar(assets, center, barWidth, health.hp, health.maxHp);
        if (int block = enemy->get<CBlock>().value; block > 0)
        {
            DrawStatIcon(assets, "Shield", {center.x - barWidth / 2.0f - statIconGap, center.y}, block);
        }
        if (int strength = enemy->get<CStrength>().value; strength > 0)
        {
            DrawStatIcon(assets, "Star", {center.x + barWidth / 2.0f + statIconGap, center.y}, strength);
        }

        if (isAlive(enemy))
        {
            const CIntent &intent = enemy->get<CIntent>();
            Vector2 intentCenter{feet.x, feet.y - 180.0f};
            Rectangle banner{intentCenter.x - intentBannerWidth / 2.0f, intentCenter.y - intentBannerHeight / 2.0f, intentBannerWidth, intentBannerHeight};
            DrawBox(assets.getBox("IntentBanner"), banner, WHITE);
            DrawIcon(assets.getTexture(IntentIcon(intent.type)), {intentCenter.x - intentIconOffset, intentCenter.y}, uiScale, WHITE);
            DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", intent.value),
                             {intentCenter.x + intentValueOffset, intentCenter.y - bigTextNudge}, bigText, textColor);
        }
    }
}

void Scene_Battle::drawFloatingTexts()
{
    const Font &font = m_game->assets().getFont("Main");
    for (const std::shared_ptr<Entity> &label : m_entityManager.getEntities("text"))
    {
        const CFloatingText &text = label->get<CFloatingText>();
        DrawTextOutlined(font, text.text.c_str(), label->get<CTransform>().pos, titleText, text.color);
    }
}

void Scene_Battle::drawHint()
{
    const Card *card = selectedCard();
    if (card == nullptr)
    {
        return;
    }
    const char *hint = "";
    switch (card->target)
    {
    case Target::SingleEnemy:
        hint = "Click an enemy";
        break;
    case Target::AllEnemies:
        hint = "Click any enemy";
        break;
    case Target::Self:
        hint = "Click your hero";
        break;
    }
    DrawTextOutlined(m_game->assets().getFont("Main"), hint, {screenWidth / 2.0f, headerY}, bigText, goldColor);
}

static void DrawPile(const Font &font, const char *label, int count, Vector2 center)
{
    DrawTextCenter(font, label, {center.x, center.y - pileLabelOffset}, smallText, peachColor);
    DrawTextOutlined(font, TextFormat("%d", count), {center.x, center.y + pileCountOffset}, titleText, textColor);
}

void Scene_Battle::drawHud()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");
    const CHealth &health = m_player->get<CHealth>();

    DrawPanel(assets, HudRect());
    DrawOrb(assets, HealthCenter(), "OrbHealth", "HealthOrbFrame", false, health.hp, health.maxHp);
    DrawOrb(assets, EnergyCenter(), "OrbEnergy", "EnergyOrbFrame", true, m_energy, startingEnergy);
    DrawPile(font, "Draw", static_cast<int>(getPile(Zone::Draw).size()), {pileInset, HealthCenter().y});
    Vector2 discardCenter{screenWidth - pileInset, EnergyCenter().y};
    DrawPile(font, "Discard", static_cast<int>(getPile(Zone::Discard).size()), discardCenter);
    int exhausted = static_cast<int>(getPile(Zone::Exhaust).size());
    if (exhausted > 0)
    {
        DrawTextCenter(font, TextFormat("Exhaust %d", exhausted), {discardCenter.x, discardCenter.y + exhaustLabelOffset}, smallText, peachColor);
    }

    EntityVec hand = getPile(Zone::Hand);
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        bool selected = hand[i] == m_selectedCard;
        bool playable = canPlayCard(hand[i], m_target);
        DrawCardSlot(assets, hand[i]->get<CCard>().id, CardRect(i, count, selected), playable, selected);
    }

    std::shared_ptr<Entity> shown = m_selectedCard ? m_selectedCard : cardAt(GetMousePosition());
    if (shown)
    {
        DrawCardTooltip(assets, shown->get<CCard>().id, {screenWidth / 2.0f, HudRect().y - tooltipGap});
    }
}

void Scene_Battle::sRender()
{
    const Assets &assets = m_game->assets();
    const Font &font = assets.getFont("Main");

    DrawBackground(assets.getTexture("Dungeon"), WHITE);
    DrawScreenShade(70);
    DrawBottomShade(floorShadeTop);
    DrawTextOutlined(font, TextFormat("Turn %d", m_turn), {screenWidth / 2.0f, turnLabelY}, bigText, peachColor);

    drawActors();
    drawActorInfo();
    drawFloatingTexts();

    if (m_result != BattleResult::InProgress && !isPlayingBack())
    {
        bool won = m_result == BattleResult::Won;
        DrawScreenShade(120);
        DrawTextOutlined(font, won ? "Victory" : "Defeat", {screenWidth / 2.0f, resultY}, hugeText, won ? goldColor : redColor);
        DrawButton(assets, ContinueButton(), "Continue", true, isHeld(ContinueButton()));
        return;
    }

    drawHud();
    DrawButton(assets, EndTurnButton(), "End Turn", true, isHeld(EndTurnButton()));
    drawHint();
}
