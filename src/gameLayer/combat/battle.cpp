#include "battle.h"
#include "combat/effects/cardEffect.h"
#include "cards/cardDatabase.h"
#include "damage.h"
#include "combat/enemies/enemyAi.h"

// Setup
Battle::Battle(PlayerState &playerState, EnemyId enemyId, std::uint32_t seed)
    : player(playerState), enemy(CreateEnemy(enemyId))
{
    deck.Init(player.masterDeck, seed);
    enemy.intent = ChooseIntent(enemy, turn);
    StartPlayerTurn();
    CheckResult();
}

// Start turn
void Battle::StartPlayerTurn()
{
    energy = startingEnergy;
    playerBlock = 0;
    deck.DrawCards(handSize);
}

// Validate card
bool Battle::CanPlayCard(int handIndex) const
{
    if (result != BattleResult::InProgress || !deck.IsValidHandIndex(handIndex))
    {
        return false;
    }

    return energy >= GetCard(deck.GetHand()[handIndex]).cost;
}

// Play card
bool Battle::PlayCard(int handIndex)
{
    if (!CanPlayCard(handIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);

    // Pay energy
    energy -= card.cost;

    // Apply effects
    ApplyCardEffects(card, {enemy, playerBlock});

    // Discard
    deck.DiscardPlayedCard(handIndex);
    CheckResult();
    return true;
}

// End turn
void Battle::EndPlayerTurn()
{
    if (result != BattleResult::InProgress)
    {
        return;
    }

    // Enemy phase
    deck.DiscardHand();
    ExecuteEnemyTurn();
    CheckResult();

    if (result != BattleResult::InProgress)
    {
        return;
    }

    // Next turn
    turn++;
    enemy.intent = ChooseIntent(enemy, turn);
    StartPlayerTurn();
}

// Execute intent
void Battle::ExecuteEnemyTurn()
{
    // Expire block
    enemy.block = 0;

    switch (enemy.intent.type)
    {
    case IntentType::Attack:
        ApplyDamage(enemy.intent.value, player.hp, playerBlock);
        break;
    case IntentType::Defend:
        enemy.block += enemy.intent.value;
        break;
    case IntentType::Buff:
        enemy.strength += enemy.intent.value;
        break;
    }
}

// Check result
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