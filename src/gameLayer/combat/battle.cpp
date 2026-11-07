#include "battle.h"
#include "cardEffect.h"
#include "cards/cardDatabase.h"
#include "damage.h"
#include "enemyAi.h"

Battle::Battle(PlayerState &playerState, EnemyId enemyId)
    : player(playerState), enemy(CreateEnemy(enemyId))
{
    deck.Init(player.masterDeck);
    enemy.intent = ChooseIntent(enemy);
    StartPlayerTurn();
    CheckResult();
}

void Battle::StartPlayerTurn()
{
    energy = startingEnergy;
    playerBlock = 0;
    deck.DrawCards(handSize);
}

bool Battle::CanPlayCard(int handIndex) const
{
    if (result != BattleResult::InProgress || !deck.IsValidHandIndex(handIndex))
    {
        return false;
    }

    return energy >= GetCard(deck.GetHand()[handIndex]).cost;
}

bool Battle::PlayCard(int handIndex)
{
    if (!CanPlayCard(handIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);
    energy -= card.cost;
    ApplyCardEffects(card, enemy, playerBlock);
    deck.DiscardPlayedCard(handIndex);
    CheckResult();
    return true;
}

void Battle::EndPlayerTurn()
{
    if (result != BattleResult::InProgress)
    {
        return;
    }

    deck.DiscardHand();
    ExecuteEnemyTurn();
    CheckResult();

    if (result != BattleResult::InProgress)
    {
        return;
    }

    turn++;
    enemy.intent = ChooseIntent(enemy);
    StartPlayerTurn();
}

void Battle::ExecuteEnemyTurn()
{
    enemy.block = 0;

    switch (enemy.intent.type)
    {
    case IntentType::Attack:
        ApplyDamage(enemy.intent.value, player.hp, playerBlock);
        break;
    }
}

void Battle::CheckResult()
{
    if (player.hp <= 0)
    {
        result = BattleResult::Lost;
    }
    else if (enemy.hp <= 0)
    {
        result = BattleResult::Won;
    }
}