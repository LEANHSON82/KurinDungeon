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

static const float barWidth = 216.0f;

static Color GetCardColor(const Card &card)
{
    for (const Effect &effect : card.effects)
    {
        if (effect.type == EffectType::Block)
        {
            return blockOutline;
        }
    }
    return damageOutline;
}

static bool IsCardTarget(const BattleScreenState &state, const Battle &battle, int actor)
{
    const Card &card = GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
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
    if (state.selectedCard != -1 && IsCardTarget(state, battle, actor))
    {
        return GetCardColor(GetCard(battle.GetDeck().GetHand()[state.selectedCard]));
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

static void DrawStats(const Assets &assets, Vector2 center, int hp, int maxHp, int block, int strength)
{
    DrawHealthBar(assets, center, barWidth, hp, maxHp);

    if (block > 0)
    {
        Vector2 icon{center.x - barWidth / 2.0f - 6.0f, center.y};
        DrawIcon(assets.shieldIcon, icon, uiScale, WHITE);
        DrawTextOutlined(assets.font, TextFormat("%d", block), {icon.x, icon.y - 4.0f}, smallText, textColor);
    }
    if (strength > 0)
    {
        Vector2 icon{center.x + barWidth / 2.0f + 6.0f, center.y};
        DrawIcon(assets.starIcon, icon, uiScale, WHITE);
        DrawTextOutlined(assets.font, TextFormat("%d", strength), {icon.x, icon.y - 4.0f}, smallText, textColor);
    }
}

static void DrawIntent(const Assets &assets, const Enemy &enemy, Vector2 center)
{
    Rectangle banner{center.x - 108.0f, center.y - 48.0f, 216.0f, 96.0f};
    DrawBox(assets.intentBanner, banner, WHITE);
    DrawIcon(GetIntentIcon(assets, enemy.intent.type), {center.x - 28.0f, center.y}, uiScale, WHITE);
    DrawTextOutlined(assets.font, TextFormat("%d", enemy.intent.value), {center.x + 32.0f, center.y - 6.0f}, bigText, textColor);
}

static void DrawActors(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    const std::vector<Enemy> &enemies = battle.GetEnemies();
    int count = static_cast<int>(enemies.size());

    DrawActor(state.player, assets.knight, GetActorFeet(playerActor, count), false, GetOutline(state, battle, playerActor));
    if (!state.player.dead && battle.GetPlayerBlock() > 0)
    {
        Vector2 icon = GetActorInfo(playerActor, count);
        DrawIcon(assets.shieldIcon, icon, uiScale, WHITE);
        DrawTextOutlined(assets.font, TextFormat("%d", battle.GetPlayerBlock()), {icon.x, icon.y - 4.0f}, smallText, textColor);
    }

    for (int i = 0; i < count; i++)
    {
        const Enemy &enemy = enemies[i];
        DrawActor(state.enemies[i], GetEnemySprites(assets, enemy.id), GetActorFeet(i, count), true, GetOutline(state, battle, i));
        if (state.enemies[i].dead)
        {
            continue;
        }

        DrawStats(assets, GetActorInfo(i, count), enemy.hp, GetEnemyDefinition(enemy.id).maxHp, enemy.block, enemy.strength);
        if (battle.IsEnemyAlive(i))
        {
            DrawIntent(assets, enemy, GetActorIntent(i, count));
        }
    }
}

static void DrawHint(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    if (state.selectedCard == -1)
    {
        return;
    }

    const char *hint = "";
    switch (GetCard(battle.GetDeck().GetHand()[state.selectedCard]).target)
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
    DrawTextOutlined(assets.font, hint, {screenWidth / 2.0f, 130.0f}, bigText, goldColor);
}

static void DrawPile(const Assets &assets, const char *label, int count, Vector2 center)
{
    DrawTextCenter(assets.font, label, {center.x, center.y - 36.0f}, smallText, peachColor);
    DrawTextOutlined(assets.font, TextFormat("%d", count), {center.x, center.y + 24.0f}, titleText, textColor);
}

static void DrawPiles(const Battle &battle, const Assets &assets)
{
    const Deck &deck = battle.GetDeck();
    DrawPile(assets, "Draw", static_cast<int>(deck.GetDrawPile().size()), {430.0f, GetHealthCenter().y});
    DrawPile(assets, "Discard", static_cast<int>(deck.GetDiscardPile().size()), {screenWidth - 430.0f, GetEnergyCenter().y});
}

static void DrawHud(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    const PlayerState &player = battle.GetPlayer();
    DrawPanel(assets, GetHudRect());
    DrawOrb(assets, GetHealthCenter(), assets.orbHealth, assets.healthOrbFrame, false, player.hp, player.maxHp);
    DrawOrb(assets, GetEnergyCenter(), assets.orbEnergy, assets.energyOrbFrame, true, battle.GetEnergy(), Battle::startingEnergy);
    DrawPiles(battle, assets);

    const std::vector<CardId> &hand = battle.GetDeck().GetHand();
    int count = static_cast<int>(hand.size());
    for (int i = 0; i < count; i++)
    {
        bool selected = i == state.selectedCard;
        DrawCardSlot(assets, hand[i], GetCardRect(i, count, selected), battle.CanPlayCard(i, state.target), selected);
    }

    int shown = state.selectedCard != -1 ? state.selectedCard : GetCardUnderMouse(battle, state.selectedCard);
    if (shown != -1)
    {
        DrawCardTooltip(assets, hand[shown], {screenWidth / 2.0f, GetHudRect().y - 12.0f});
    }
}

void DrawBattleScreen(const BattleScreenState &state, const Battle &battle, const Assets &assets)
{
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(70);
    DrawBottomShade(560.0f);
    DrawTextOutlined(assets.font, TextFormat("Turn %d", battle.GetTurn()), {screenWidth / 2.0f, 54.0f}, bigText, peachColor);

    DrawActors(state, battle, assets);

    for (const FloatingText &text : state.texts)
    {
        DrawTextOutlined(assets.font, text.text.c_str(), text.position, titleText, text.color);
    }

    bool finished = battle.GetResult() != BattleResult::InProgress && !IsPlayingBack(state);
    if (finished)
    {
        bool won = battle.GetResult() == BattleResult::Won;
        DrawScreenShade(120);
        DrawTextOutlined(assets.font, won ? "Victory" : "Defeat", {screenWidth / 2.0f, 400.0f}, hugeText, won ? goldColor : redColor);
        DrawButton(assets, GetContinueButton(), "Continue", true);
        return;
    }

    DrawHud(state, battle, assets);
    DrawButton(assets, GetEndTurnButton(), "End Turn", true);
    DrawHint(state, battle, assets);
}
