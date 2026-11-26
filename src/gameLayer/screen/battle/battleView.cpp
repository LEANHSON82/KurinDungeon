#include <vector>
#include <raylib.h>
#include "battleScreen.h"
#include "battleLayout.h"
#include "battlePlayback.h"
#include "cards/cardDatabase.h"
#include "combat/enemies/enemyDatabase.h"
#include "gameConfig.h"
#include "screen/cardView.h"
#include "screen/ui/image.h"
#include "screen/ui/style.h"
#include "screen/ui/text.h"
#include "screen/ui/widget.h"

const float barWidth = 216.0f;
const float statIconGap = 6.0f;
const float statusRowHeight = 250.0f;
const float statusBadgeWidth = 110.0f;
const float statusIconOffset = 34.0f;
const float statusValueOffset = 82.0f;
const float statusIconScale = 4.0f;
const float infoBelowFeet = 46.0f;
const float intentHeight = 180.0f;
const float intentBannerWidth = 216.0f;
const float intentBannerHeight = 96.0f;
const float intentIconOffset = 28.0f;
const float intentValueOffset = 32.0f;
const float pileInset = 430.0f;
const float pileLabelOffset = 36.0f;
const float pileCountOffset = 24.0f;
const float exhaustLabelOffset = 82.0f;
const float tooltipGap = 12.0f;
const float floorShadeTop = 560.0f;
const float turnLabelY = 54.0f;
const float resultY = 400.0f;
const int hudFadeHeight = 90;
const int hudRuleHeight = 4;
const Color hudBarColor{18, 12, 26, 240};
const Color hudRuleColor{240, 181, 65, 255};

static const Card *GetSelectedCard(const BattleScreenState &state, const Battle &battle)
{
    if (state.selectedCard == -1)
    {
        return nullptr;
    }
    return &GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
}

static Color GetCardColor(const Card &card)
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

static bool IsCardTarget(const BattleScreenState &state, const Battle &battle, const Card &card, int actor)
{
    switch (card.target)
    {
    case Target::SingleEnemy:
        return actor == state.target && battle.IsEnemyAlive(actor);
    case Target::AllEnemies:
        return actor != playerActor && battle.IsEnemyAlive(actor);
    case Target::Self:
        return actor == playerActor;
    }
    return false;
}

static Color GetOutline(const BattleScreenState &state, const Battle &battle, int actor)
{
    const Card *card = GetSelectedCard(state, battle);
    if (card != nullptr && IsCardTarget(state, battle, *card, actor))
    {
        return GetCardColor(*card);
    }
    return outlineColor;
}

static void DrawActor(const ActorView &actor, const CharacterSprites &sprites, Vector2 feet, bool flip, Color outline)
{
    const SpriteSheet &sheet = GetAnimation(sprites, actor.animation);
    int frame = GetActorFrame(actor, sheet);
    if (!actor.dead)
    {
        DrawSpriteOutline(sheet, frame, feet, flip, outline);
    }
    DrawSprite(sheet, frame, feet, flip, WHITE);
}

static void DrawActors(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    int enemyCount = static_cast<int>(battle.GetEnemies().size());
    DrawActor(state.player, GetActorSprites(battle, assets, playerActor), GetActorFeet(playerActor, enemyCount), false,
              GetOutline(state, battle, playerActor));
    for (int i = 0; i < enemyCount; i++)
    {
        DrawActor(state.enemies[i], GetActorSprites(battle, assets, i), GetActorFeet(i, enemyCount), true, GetOutline(state, battle, i));
    }
}

static void DrawStatIcon(const Assets &assets, Texture2D icon, Vector2 center, int value)
{
    DrawIcon(icon, center, iconScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d", value), {center.x, center.y - smallTextNudge}, smallText, textColor);
}

static Texture2D GetIntentIcon(const Assets &assets, const Enemy &enemy)
{
    switch (enemy.intent.type)
    {
    case IntentType::Attack:
        return assets.swordIcon;
    case IntentType::Defend:
        return assets.shieldIcon;
    case IntentType::Buff:
        return GetStatusIcon(assets, StatusType::Strength);
    case IntentType::Debuff:
        return GetStatusIcon(assets, GetEnemyDefinition(enemy.id).debuff);
    }
    return assets.swordIcon;
}

static Vector2 GetInfoPoint(Vector2 feet)
{
    return {feet.x, feet.y + infoBelowFeet};
}

static Vector2 GetStatusPoint(Vector2 feet)
{
    return {feet.x, feet.y - statusRowHeight};
}

static void DrawStatuses(const Assets &assets, const Statuses &statuses, Vector2 center)
{
    std::vector<StatusType> active;
    for (int i = 0; i < statusCount; i++)
    {
        if (GetStatus(statuses, static_cast<StatusType>(i)) > 0)
        {
            active.push_back(static_cast<StatusType>(i));
        }
    }

    float x = center.x - active.size() * statusBadgeWidth / 2.0f;
    for (StatusType type : active)
    {
        DrawIcon(GetStatusIcon(assets, type), {x + statusIconOffset, center.y}, statusIconScale, WHITE);
        DrawTextOutlined(assets.font, TextFormat("%d", GetStatus(statuses, type)), {x + statusValueOffset, center.y - smallTextNudge}, smallText, textColor);
        x += statusBadgeWidth;
    }
}

static void DrawPlayerInfo(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    if (state.player.dead)
    {
        return;
    }
    Vector2 feet = GetActorFeet(playerActor, static_cast<int>(battle.GetEnemies().size()));
    if (battle.GetPlayerBlock() > 0)
    {
        DrawStatIcon(assets, assets.shieldIcon, GetInfoPoint(feet), battle.GetPlayerBlock());
    }
    DrawStatuses(assets, battle.GetStatuses(playerActor), GetStatusPoint(feet));
}

static void DrawIntent(const Battle &battle, const Assets &assets, int index, Vector2 feet)
{
    const Enemy &enemy = battle.GetEnemies()[index];
    Vector2 center{feet.x, feet.y - intentHeight};
    Rectangle banner{center.x - intentBannerWidth / 2.0f, center.y - intentBannerHeight / 2.0f, intentBannerWidth, intentBannerHeight};
    DrawBox(assets.intentBanner, banner, WHITE);

    int value = enemy.intent.value;
    if (enemy.intent.type == IntentType::Attack)
    {
        value = battle.ModifyDamage(index, playerActor, enemy.intent.value);
    }
    DrawIcon(GetIntentIcon(assets, enemy), {center.x - intentIconOffset, center.y}, iconScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d", value), {center.x + intentValueOffset, center.y - bigTextNudge}, bigText, textColor);
}

static void DrawEnemyInfo(const BattleScreenState &state, const Battle &battle, const Assets &assets, int index)
{
    if (state.enemies[index].dead)
    {
        return;
    }
    const Enemy &enemy = battle.GetEnemies()[index];
    Vector2 feet = GetActorFeet(index, static_cast<int>(battle.GetEnemies().size()));
    Vector2 center = GetInfoPoint(feet);
    DrawHealthBar(assets, center, barWidth, enemy.hp, GetEnemyDefinition(enemy.id).maxHp);

    if (enemy.block > 0)
    {
        DrawStatIcon(assets, assets.shieldIcon, {center.x - barWidth / 2.0f - statIconGap, center.y}, enemy.block);
    }
    DrawStatuses(assets, enemy.status, GetStatusPoint(feet));

    if (battle.IsEnemyAlive(index))
    {
        DrawIntent(battle, assets, index, feet);
    }
}

static void DrawFloatingTexts(const BattleScreenState &state, const Assets &assets)
{
    for (const FloatingText &text : state.texts)
    {
        DrawTextOutlined(assets.font, text.text.c_str(), text.position, titleText, text.color);
    }
}

static void DrawHint(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    const Card *card = GetSelectedCard(state, battle);
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

static void DrawPile(const Font &font, const char *label, int count, Vector2 center)
{
    DrawTextCenter(font, label, {center.x, center.y - pileLabelOffset}, smallText, peachColor);
    DrawTextOutlined(font, TextFormat("%d", count), {center.x, center.y + pileCountOffset}, titleText, textColor);
}

static void DrawPiles(const Battle &battle, const Assets &assets)
{
    const Deck &deck = battle.GetDeck();
    DrawPile(assets.font, "Draw", static_cast<int>(deck.GetDrawPile().size()), {pileInset, GetHealthCenter().y});
    Vector2 discardCenter{screenWidth - pileInset, GetEnergyCenter().y};
    DrawPile(assets.font, "Discard", static_cast<int>(deck.GetDiscardPile().size()), discardCenter);

    int exhausted = static_cast<int>(deck.GetExhaustPile().size());
    if (exhausted > 0)
    {
        DrawTextCenter(assets.font, TextFormat("Exhaust %d", exhausted), {discardCenter.x, discardCenter.y + exhaustLabelOffset}, smallText, peachColor);
    }
}

static void DrawHand(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    const std::vector<CardId> &hand = battle.GetDeck().GetHand();
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        bool selected = i == state.selectedCard;
        bool playable = battle.CanPlayCard(i, state.target);
        DrawCardSlot(assets, hand[i], GetCardRect(i, count, selected), playable, selected);
    }
}

static void DrawHandTooltip(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    int shown = state.selectedCard;
    if (shown == -1)
    {
        shown = GetCardUnderMouse(battle, state.selectedCard);
    }
    if (shown == -1)
    {
        return;
    }

    CardId id = battle.GetDeck().GetHand()[shown];
    Statuses victim;
    if (GetCard(id).target == Target::SingleEnemy && battle.IsEnemyAlive(state.target))
    {
        victim = battle.GetStatuses(state.target);
    }
    DrawCardTooltip(assets, id, {screenWidth / 2.0f, hudTop - tooltipGap}, battle.GetStatuses(playerActor), victim);
}

static void DrawHudBar()
{
    int top = static_cast<int>(hudTop);
    Color clear{hudBarColor.r, hudBarColor.g, hudBarColor.b, 0};
    DrawRectangleGradientV(0, top - hudFadeHeight, screenWidth, hudFadeHeight, clear, hudBarColor);
    DrawRectangle(0, top, screenWidth, screenHeight - top, hudBarColor);
    DrawRectangle(0, top, screenWidth, hudRuleHeight, hudRuleColor);
    DrawRectangle(0, top + hudRuleHeight, screenWidth, hudRuleHeight, inkColor);
}

static void DrawHud(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    const PlayerState &player = battle.GetPlayer();
    DrawHudBar();
    DrawOrb(assets, GetHealthCenter(), assets.orbHealth, assets.healthOrbFrame, false, player.hp, player.maxHp);
    DrawOrb(assets, GetEnergyCenter(), assets.orbEnergy, assets.energyOrbFrame, true, battle.GetEnergy(), Battle::startingEnergy);
    DrawPiles(battle, assets);
    DrawHand(state, battle, assets);
    DrawHandTooltip(state, battle, assets);
}

void DrawBattleScreen(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    DrawBackground(assets.dungeon, WHITE);
    DrawScreenShade(70);
    DrawBottomShade(floorShadeTop);
    DrawTextOutlined(assets.font, TextFormat("Turn %d", battle.GetTurn()), {screenWidth / 2.0f, turnLabelY}, bigText, peachColor);

    DrawActors(state, battle, assets);
    DrawPlayerInfo(state, battle, assets);
    for (int i = 0; i < static_cast<int>(battle.GetEnemies().size()); i++)
    {
        DrawEnemyInfo(state, battle, assets, i);
    }
    DrawFloatingTexts(state, assets);

    if (battle.GetResult() != BattleResult::InProgress && !IsPlayingBack(state))
    {
        bool won = battle.GetResult() == BattleResult::Won;
        DrawScreenShade(120);
        DrawHeading(assets.titleFont, won ? "Victory" : "Defeat", {screenWidth / 2.0f, resultY}, bannerText, won ? goldColor : redColor);
        DrawButton(assets, GetContinueButton(), "Continue", true);
        return;
    }

    DrawHud(state, battle, assets);
    DrawButton(assets, GetEndTurnButton(), "End Turn", true);
    DrawHint(state, battle, assets);
}
