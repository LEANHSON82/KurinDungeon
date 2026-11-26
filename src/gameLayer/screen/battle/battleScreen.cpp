#include <raylib.h>
#include "battleScreen.h"
#include "battleLayout.h"
#include "battleLog.h"
#include "battlePlayback.h"
#include "cards/cardDatabase.h"
#include "screen/ui/widget.h"

const int selectKeyCount = 9;

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
    const Card &card = GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
    if (!battle.PlayCard(state.selectedCard, target))
    {
        return;
    }
    LogBattle(battle, TextFormat("PlayCard %s", card.name));
    PlayGameSound(assets.playCard);
    state.selectedCard = -1;
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

static void ConfirmSelectedCard(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    if (state.selectedCard == -1)
    {
        return;
    }
    const Card &card = GetCard(battle.GetDeck().GetHand()[state.selectedCard]);
    PlaySelectedCard(state, battle, assets, card.target == Target::SingleEnemy ? state.target : -1);
}

static bool UpdateKeyboard(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    if (IsKeyPressed(KEY_RIGHT))
    {
        state.target = NextLivingEnemy(battle, state.target, 1);
        return true;
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        state.target = NextLivingEnemy(battle, state.target, -1);
        return true;
    }
    if (IsKeyPressed(KEY_ENTER))
    {
        ConfirmSelectedCard(state, battle, assets);
        return true;
    }

    int handCount = static_cast<int>(battle.GetDeck().GetHand().size());
    for (int i = 0; i < selectKeyCount && i < handCount; i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
        {
            ClickCard(state, i);
            return true;
        }
    }
    return false;
}

static void UpdateMouse(BattleScreenState &state, Battle &battle, const Assets &assets)
{
    Vector2 delta = GetMouseDelta();
    int enemy = GetEnemyUnderMouse(battle);
    if (enemy != -1 && (delta.x != 0.0f || delta.y != 0.0f))
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
    if (!battle.IsEnemyAlive(state.target))
    {
        state.target = NextLivingEnemy(battle, state.target, 1);
    }
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

    if (IsKeyPressed(KEY_E) || IsClicked(GetEndTurnButton()))
    {
        battle.EndPlayerTurn();
        state.selectedCard = -1;
        LogBattle(battle, "EndTurn");
        return ScreenAction::None;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        state.selectedCard = -1;
        return ScreenAction::None;
    }

    if (UpdateKeyboard(state, battle, assets))
    {
        return ScreenAction::None;
    }
    UpdateMouse(state, battle, assets);
    return ScreenAction::None;
}
