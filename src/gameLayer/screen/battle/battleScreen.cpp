#include <raylib.h>
#include "battleScreen.h"
#include "battleLayout.h"
#include "battleLog.h"
#include "battlePlayback.h"
#include "cards/cardDatabase.h"
#include "screen/ui/widget.h"

static int NextLivingEnemy(const Battle &battle, int from, int step)
{
    int count = static_cast<int>(battle.GetEnemies().size());
    int index = from;
    for (int tries = 0; tries < count; tries++)
    {
        index += step;
        if (index < 0)
        {
            index = count - 1;
        }
        if (index >= count)
        {
            index = 0;
        }
        if (battle.IsEnemyAlive(index))
        {
            return index;
        }
    }
    return from;
}

static void PlaySelectedCard(BattleScreenState &state, Battle &battle, const Assets &assets, int target)
{
    int card = state.selectedCard;
    bool played = battle.PlayCard(card, target);
    LogBattle(battle, TextFormat("PlayCard(%d, target %d) %s", card, target, played ? "accepted" : "rejected"));
    if (played)
    {
        PlayGameSound(assets.playCard);
        state.selectedCard = -1;
    }
}

static void ClickCard(BattleScreenState &state, int card)
{
    state.selectedCard = card == state.selectedCard ? -1 : card;
}

static void ClickBattlefield(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    const Card &card = GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
    int enemy = GetEnemyUnderMouse(battle);
    int enemyCount = static_cast<int>(battle.GetEnemies().size());

    switch (card.target)
    {
    case Target::SingleEnemy:
        if (enemy != -1)
        {
            PlaySelectedCard(state, battle, assets, enemy);
        }
        break;
    case Target::AllEnemies:
        if (enemy != -1)
        {
            PlaySelectedCard(state, battle, assets, -1);
        }
        break;
    case Target::Self:
        if (IsHovered(GetActorRect(playerActor, enemyCount)))
        {
            PlaySelectedCard(state, battle, assets, -1);
        }
        break;
    }
}

static void UpdateKeyboard(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    if (IsKeyPressed(KEY_RIGHT))
    {
        state.target = NextLivingEnemy(battle, state.target, 1);
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        state.target = NextLivingEnemy(battle, state.target, -1);
    }

    for (int i = 0; i < static_cast<int>(battle.GetDeck().GetHand().size()); i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
        {
            ClickCard(state, i);
        }
    }

    if (IsKeyPressed(KEY_ENTER) && state.selectedCard != -1)
    {
        const Card &card = GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
        PlaySelectedCard(state, battle, assets, card.target == Target::SingleEnemy ? state.target : -1);
    }
}

static void UpdateMouse(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    int enemy = GetEnemyUnderMouse(battle);
    if (enemy != -1)
    {
        state.target = enemy;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return;
    }

    int card = GetCardUnderMouse(battle, state.selectedCard);
    if (card != -1)
    {
        ClickCard(state, card);
    }
    else if (state.selectedCard != -1)
    {
        ClickBattlefield(state, battle, assets);
    }
}

ScreenAction UpdateBattleScreen(BattleScreenState &state, Battle &battle, const Assets &assets, float deltaTime)
{
    UpdatePlayback(state, battle, assets, deltaTime);
    if (IsPlayingBack(state))
    {
        return ScreenAction::None;
    }

    if (battle.GetResult() != BattleResult::InProgress)
    {
        if (IsKeyPressed(KEY_ENTER) || IsClicked(GetContinueButton()))
        {
            return ScreenAction::LeaveBattle;
        }
        return ScreenAction::None;
    }

    if (!battle.IsEnemyAlive(state.target))
    {
        state.target = NextLivingEnemy(battle, state.target, 1);
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        state.selectedCard = -1;
        return ScreenAction::None;
    }

    if (IsKeyPressed(KEY_E) || IsClicked(GetEndTurnButton()))
    {
        LogBattle(battle, "Before EndTurn");
        battle.EndPlayerTurn();
        state.selectedCard = -1;
        LogBattle(battle, "After EndTurn");
        return ScreenAction::None;
    }

    UpdateKeyboard(state, battle, assets);
    UpdateMouse(state, battle, assets);
    return ScreenAction::None;
}
