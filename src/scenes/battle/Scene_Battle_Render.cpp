#include "scenes/battle/Scene_Battle.h"
#include "engine/GameEngine.h"
#include "scenes/battle/BattleLayout.h"
#include "ui/CardView.h"
#include "ui/Image.h"
#include "ui/Text.h"
#include "ui/Widget.h"

static constexpr float barWidth = 216.0f;
static constexpr float statIconGap = 6.0f;
static constexpr float statusRowHeight = 250.0f; // above the head, over the intent banner
static constexpr float statusBadgeWidth = 110.0f; // one icon and its number
static constexpr float statusIconOffset = 34.0f;  // from the left edge of a badge
static constexpr float statusValueOffset = 82.0f;
static constexpr float statusIconScale = 4.0f;
static constexpr float infoBelowFeet = 46.0f; // health bar and Block sit just under the feet
static constexpr float intentHeight = 180.0f; // the intent banner floats above the head
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

void Scene_Battle::drawActor(const std::shared_ptr<Entity> &actor)
{
    const CAnimation &animation = actor->get<CAnimation>();
    const CActor &data = actor->get<CActor>();
    std::size_t frame = animation.animation.frame(animation.repeat);
    Vector2 feet = actor->get<CTransform>().pos;
    if (!data.fallen)
    {
        DrawSpriteOutline(animation.animation, frame, feet, data.flip, outlineFor(actor));
    }
    DrawSprite(animation.animation, frame, feet, data.flip, WHITE);
}

void Scene_Battle::drawActors()
{
    drawActor(m_player);
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        drawActor(enemy);
    }
}

static void DrawStatIcon(const Assets &assets, const char *icon, Vector2 center, int value)
{
    DrawIcon(assets.getTexture(icon), center, uiScale, WHITE);
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", value), {center.x, center.y - smallTextNudge}, smallText, textColor);
}

static const char *IntentIcon(const std::shared_ptr<Entity> &enemy, IntentType type)
{
    switch (type)
    {
    case IntentType::Attack:
        return "Sword";
    case IntentType::Defend:
        return "Shield";
    case IntentType::Buff:
        return GetStatusDefinition(StatusType::Strength).icon;
    case IntentType::Debuff:
        return GetStatusDefinition(GetEnemyDefinition(enemy->get<CEnemy>().id).debuff).icon;
    }
    return "Sword";
}

static Vector2 InfoPoint(Vector2 feet)
{
    return {feet.x, feet.y + infoBelowFeet};
}

static Vector2 StatusPoint(Vector2 feet)
{
    return {feet.x, feet.y - statusRowHeight};
}

// One icon and number per active status, centered over an actor's head.
void Scene_Battle::drawStatuses(const std::shared_ptr<Entity> &actor, Vector2 center)
{
    const Assets &assets = m_game->assets();
    const CStatus &status = actor->get<CStatus>();
    std::vector<StatusType> active;
    for (int i = 0; i < statusCount; i++)
    {
        if (status.amount(static_cast<StatusType>(i)) > 0)
        {
            active.push_back(static_cast<StatusType>(i));
        }
    }

    float x = center.x - active.size() * statusBadgeWidth / 2.0f;
    for (StatusType type : active)
    {
        DrawIcon(assets.getTexture(GetStatusDefinition(type).icon), {x + statusIconOffset, center.y}, statusIconScale, WHITE);
        DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", status.amount(type)),
                         {x + statusValueOffset, center.y - smallTextNudge}, smallText, textColor);
        x += statusBadgeWidth;
    }
}

void Scene_Battle::drawPlayerInfo()
{
    if (m_player->get<CActor>().fallen)
    {
        return;
    }
    Vector2 feet = m_player->get<CTransform>().pos;
    int block = m_player->get<CBlock>().value;
    if (block > 0)
    {
        DrawStatIcon(m_game->assets(), "Shield", InfoPoint(feet), block);
    }
    drawStatuses(m_player, StatusPoint(feet));
}

void Scene_Battle::drawIntent(const std::shared_ptr<Entity> &enemy)
{
    const Assets &assets = m_game->assets();
    const CIntent &intent = enemy->get<CIntent>();
    Vector2 feet = enemy->get<CTransform>().pos;
    Vector2 center{feet.x, feet.y - intentHeight};
    Rectangle banner{center.x - intentBannerWidth / 2.0f, center.y - intentBannerHeight / 2.0f, intentBannerWidth, intentBannerHeight};
    DrawBox(assets.getBox("IntentBanner"), banner, WHITE);

    // An attack shows the damage it will really deal after statuses.
    int value = intent.value;
    if (intent.type == IntentType::Attack)
    {
        value = modifyDamage(enemy, m_player, intent.value);
    }
    DrawIcon(assets.getTexture(IntentIcon(enemy, intent.type)), {center.x - intentIconOffset, center.y}, uiScale, WHITE);
    DrawTextOutlined(assets.getFont("Main"), TextFormat("%d", value), {center.x + intentValueOffset, center.y - bigTextNudge}, bigText, textColor);
}

void Scene_Battle::drawEnemyInfo(const std::shared_ptr<Entity> &enemy)
{
    if (enemy->get<CActor>().fallen)
    {
        return;
    }
    const Assets &assets = m_game->assets();
    Vector2 feet = enemy->get<CTransform>().pos;
    Vector2 center = InfoPoint(feet);
    const CHealth &health = enemy->get<CHealth>();
    DrawHealthBar(assets, center, barWidth, health.hp, health.maxHp);

    int block = enemy->get<CBlock>().value;
    if (block > 0)
    {
        DrawStatIcon(assets, "Shield", {center.x - barWidth / 2.0f - statIconGap, center.y}, block);
    }
    drawStatuses(enemy, StatusPoint(feet));

    // A dying enemy keeps its health bar until it has fallen, but no longer plans anything.
    if (isAlive(enemy))
    {
        drawIntent(enemy);
    }
}

void Scene_Battle::drawActorInfo()
{
    drawPlayerInfo();
    for (const std::shared_ptr<Entity> &enemy : getEnemies())
    {
        drawEnemyInfo(enemy);
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

void Scene_Battle::drawPiles()
{
    const Font &font = m_game->assets().getFont("Main");
    DrawPile(font, "Draw", pileSize(Zone::Draw), {pileInset, HealthCenter().y});
    Vector2 discardCenter{screenWidth - pileInset, EnergyCenter().y};
    DrawPile(font, "Discard", pileSize(Zone::Discard), discardCenter);

    int exhausted = pileSize(Zone::Exhaust);
    if (exhausted > 0)
    {
        DrawTextCenter(font, TextFormat("Exhaust %d", exhausted), {discardCenter.x, discardCenter.y + exhaustLabelOffset}, smallText, peachColor);
    }
}

void Scene_Battle::drawHand()
{
    EntityVec hand = getPile(Zone::Hand);
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        bool selected = hand[i] == m_selectedCard;
        bool playable = canPlayCard(hand[i], m_target);
        DrawCardSlot(m_game->assets(), hand[i]->get<CCard>().id, CardRect(i, count, selected), playable, selected);
    }
}

// Describes the selected card, or else the card under the mouse.
void Scene_Battle::drawCardTooltip()
{
    std::shared_ptr<Entity> shown = m_selectedCard;
    if (!shown)
    {
        shown = cardAt(GetMousePosition());
    }
    if (!shown)
    {
        return;
    }

    // Damage in the text includes Strength, Weak and the current target's Vulnerable.
    const Card &card = GetCard(shown->get<CCard>().id);
    std::shared_ptr<Entity> target;
    if (card.target == Target::SingleEnemy && isAlive(m_target))
    {
        target = m_target;
    }
    auto damage = [&](int base) { return modifyDamage(m_player, target, base); };
    DrawCardTooltip(m_game->assets(), shown->get<CCard>().id, {screenWidth / 2.0f, HudRect().y - tooltipGap}, damage);
}

void Scene_Battle::drawHud()
{
    const Assets &assets = m_game->assets();
    const CHealth &health = m_player->get<CHealth>();

    DrawPanel(assets, HudRect());
    DrawOrb(assets, HealthCenter(), "OrbHealth", "HealthOrbFrame", false, health.hp, health.maxHp);
    DrawOrb(assets, EnergyCenter(), "OrbEnergy", "EnergyOrbFrame", true, m_energy, startingEnergy);
    drawPiles();
    drawHand();
    drawCardTooltip();
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
        DrawButton(assets, ContinueButton(), "Continue", ButtonStyle::Gold, isHeld(ContinueButton()));
        return;
    }

    drawHud();
    DrawButton(assets, EndTurnButton(), "End Turn", ButtonStyle::Gold, isHeld(EndTurnButton()));
    drawHint();
}
