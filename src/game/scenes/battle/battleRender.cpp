#include <raylib.h>
#include "core/components/cards.h"
#include "core/components/combat.h"
#include "core/state/battle.h"
#include "core/systems/battle/combatRules.h"
#include "game/assets/assets.h"
#include "game/components/visual.h"
#include "game/gameConfig.h"
#include "game/scenes/battle/battleLayout.h"
#include "game/scenes/scenes.h"
#include "game/state/battleView.h"
#include "game/systems/animationSystem.h"
#include "game/systems/playbackSystem.h"
#include "game/ui/cardView.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static constexpr float barWidth = 216.0f;
static constexpr float statIconGap = 6.0f;
static constexpr float intentBannerWidth = 216.0f;
static constexpr float intentBannerHeight = 96.0f;
static constexpr float intentIconOffset = 28.0f;
static constexpr float intentValueOffset = 32.0f;
static constexpr float pileInset = 430.0f;
static constexpr float pileLabelOffset = 36.0f;
static constexpr float pileCountOffset = 24.0f;
static constexpr float tooltipGap = 12.0f;
static constexpr float floorShadeTop = 560.0f;
static constexpr float turnLabelY = 54.0f;
static constexpr float resultY = 400.0f;

static Color GetCardColor(const Card &card)
{
    switch (card.kind)
    {
    case CardKind::Attack:
        return damageOutline;
    case CardKind::Skill:
        return blockOutline;
    }
    return damageOutline;
}

static bool IsCardTarget(const ecs::Registry &registry, const Card &card, ecs::Entity actor)
{
    switch (card.target)
    {
    case Target::SingleEnemy:
        return actor == registry.Resource<BattleUi>().target && IsEnemyAlive(registry, actor);
    case Target::AllEnemies:
        return IsEnemyAlive(registry, actor);
    case Target::Self:
        return registry.Has<Player>(actor);
    }
    return false;
}

static Color GetOutline(const ecs::Registry &registry, ecs::Entity actor)
{
    const Card *card = GetSelectedCard(registry);
    if (card != nullptr && IsCardTarget(registry, *card, actor))
    {
        return GetCardColor(*card);
    }
    return outlineColor;
}

static void DrawActorSprites(const ecs::Registry &registry)
{
    registry.View<Position, Sprite, Animator>().Each(
        [&](ecs::Entity entity, const Position &position, const Sprite &sprite, const Animator &animator) {
            const SpriteSheet &sheet = GetAnimation(*sprite.sprites, animator.animation);
            int frame = GetAnimationFrame(animator, sheet);
            if (!animator.dead)
            {
                DrawSpriteOutline(sheet, frame, position.value, sprite.flip, GetOutline(registry, entity));
            }
            DrawSprite(sheet, frame, position.value, sprite.flip, WHITE);
        });
}

static void DrawStatIcon(const Assets &assets, Texture2D icon, Vector2 center, int value)
{
    DrawIcon(icon, center, uiScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d", value), {center.x, center.y - smallTextNudge}, smallText, textColor);
}

static void DrawEnemyStats(const Assets &assets, Vector2 center, const Health &health, int block, int strength)
{
    DrawHealthBar(assets, center, barWidth, health.hp, health.maxHp);
    if (block > 0)
    {
        DrawStatIcon(assets, assets.shieldIcon, {center.x - barWidth / 2.0f - statIconGap, center.y}, block);
    }
    if (strength > 0)
    {
        DrawStatIcon(assets, assets.starIcon, {center.x + barWidth / 2.0f + statIconGap, center.y}, strength);
    }
}

static void DrawIntent(const Assets &assets, const Intent &intent, Vector2 center)
{
    Rectangle banner{center.x - intentBannerWidth / 2.0f, center.y - intentBannerHeight / 2.0f, intentBannerWidth, intentBannerHeight};
    DrawBox(assets.intentBanner, banner, WHITE);
    DrawIcon(GetIntentIcon(assets, intent.type), {center.x - intentIconOffset, center.y}, uiScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d", intent.value), {center.x + intentValueOffset, center.y - bigTextNudge}, bigText, textColor);
}

static void DrawActorInfo(const ecs::Registry &registry, const Assets &assets)
{
    ecs::Entity player = GetPlayer(registry);
    int playerBlock = registry.Get<Block>(player).value;
    if (!registry.Get<Animator>(player).dead && playerBlock > 0)
    {
        DrawStatIcon(assets, assets.shieldIcon, GetInfoPoint(registry.Get<Position>(player).value), playerBlock);
    }

    for (ecs::Entity enemy : GetEnemiesInOrder(registry))
    {
        if (registry.Get<Animator>(enemy).dead)
        {
            continue;
        }
        Vector2 feet = registry.Get<Position>(enemy).value;
        DrawEnemyStats(assets, GetInfoPoint(feet), registry.Get<Health>(enemy),
                       registry.Get<Block>(enemy).value, registry.Get<Strength>(enemy).value);
        if (IsEnemyAlive(registry, enemy))
        {
            DrawIntent(assets, registry.Get<Intent>(enemy), GetIntentPoint(feet));
        }
    }
}

static void DrawFloatingTexts(const ecs::Registry &registry, const Assets &assets)
{
    registry.View<FloatingText, Position>().Each([&](ecs::Entity, const FloatingText &text, const Position &position) {
        DrawTextOutlined(assets.font, text.text.c_str(), position.value, titleText, text.color);
    });
}

static void DrawHint(const ecs::Registry &registry, const Assets &assets)
{
    const Card *card = GetSelectedCard(registry);
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
    DrawTextOutlined(assets.font, hint, {screenWidth / 2.0f, headerY}, bigText, goldColor);
}

static void DrawPile(const Assets &assets, const char *label, int count, Vector2 center)
{
    DrawTextCenter(assets.font, label, {center.x, center.y - pileLabelOffset}, smallText, peachColor);
    DrawTextOutlined(assets.font, TextFormat("%d", count), {center.x, center.y + pileCountOffset}, titleText, textColor);
}

static void DrawHud(const ecs::Registry &registry, const Assets &assets)
{
    const BattleUi &ui = registry.Resource<BattleUi>();
    const Health &health = registry.Get<Health>(GetPlayer(registry));

    DrawPanel(assets, GetHudRect());
    DrawOrb(assets, GetHealthCenter(), assets.orbHealth, assets.healthOrbFrame, false, health.hp, health.maxHp);
    DrawOrb(assets, GetEnergyCenter(), assets.orbEnergy, assets.energyOrbFrame, true, registry.Resource<BattleState>().energy, startingEnergy);
    DrawPile(assets, "Draw", static_cast<int>(GetPile(registry, Zone::Draw).size()), {pileInset, GetHealthCenter().y});
    DrawPile(assets, "Discard", static_cast<int>(GetPile(registry, Zone::Discard).size()), {screenWidth - pileInset, GetEnergyCenter().y});

    std::vector<ecs::Entity> hand = GetPile(registry, Zone::Hand);
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        bool selected = hand[i] == ui.selectedCard;
        bool playable = CanPlayCard(registry, hand[i], ui.target);
        DrawCardSlot(assets, registry.Get<DeckCard>(hand[i]).id, GetCardRect(i, count, selected), playable, selected);
    }

    ecs::Entity shown = !ui.selectedCard.IsNull() ? ui.selectedCard : GetCardUnderMouse(hand, ui.selectedCard);
    if (!shown.IsNull())
    {
        DrawCardTooltip(assets, registry.Get<DeckCard>(shown).id, {screenWidth / 2.0f, GetHudRect().y - tooltipGap});
    }
}

void DrawBattleScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    const BattleState &battle = registry.Resource<BattleState>();

    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(70);
    DrawBottomShade(floorShadeTop);
    DrawTextOutlined(assets.font, TextFormat("Turn %d", battle.turn), {screenWidth / 2.0f, turnLabelY}, bigText, peachColor);

    DrawActorSprites(registry);
    DrawActorInfo(registry, assets);
    DrawFloatingTexts(registry, assets);

    bool finished = battle.result != BattleResult::InProgress && !IsPlayingBack(registry);
    if (finished)
    {
        bool won = battle.result == BattleResult::Won;
        DrawScreenShade(120);
        DrawTextOutlined(assets.font, won ? "Victory" : "Defeat", {screenWidth / 2.0f, resultY}, hugeText, won ? goldColor : redColor);
        DrawButton(assets, GetContinueButton(), "Continue", true);
        return;
    }

    DrawHud(registry, assets);
    DrawButton(assets, GetEndTurnButton(), "End Turn", true);
    DrawHint(registry, assets);
}
