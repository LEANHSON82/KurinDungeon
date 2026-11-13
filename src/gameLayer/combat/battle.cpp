#include "battle.h"
#include "combat/effects/cardEffect.h"
#include "cards/cardDatabase.h"
#include "damage.h"
#include "combat/enemies/enemyAi.h"

// Setup
Battle::Battle(PlayerState &playerState, const std::vector<EnemyId> &enemyIds, std::uint32_t seed)
    : player(playerState)
{
    for (EnemyId id : enemyIds)
    {
        enemies.push_back(CreateEnemy(id));
    }

    deck.Init(player.masterDeck, seed);
    ChooseEnemyIntents();
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

// Enemy alive
bool Battle::IsEnemyAlive(int index) const
{
    return index >= 0 && index < static_cast<int>(enemies.size()) && enemies[index].hp > 0;
}

// Validate card
bool Battle::CanPlayCard(int handIndex, int targetIndex) const
{
    if (result != BattleResult::InProgress || !deck.IsValidHandIndex(handIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);
    if (card.target == Target::SingleEnemy && !IsEnemyAlive(targetIndex))
    {
        return false;
    }

    return energy >= card.cost;
}

// Card targets
std::vector<Enemy *> Battle::GetTargets(const Card &card, int targetIndex)
{
    std::vector<Enemy *> targets;
    switch (card.target)
    {
    case Target::Self:
        break;
    case Target::SingleEnemy:
        targets.push_back(&enemies[targetIndex]);
        break;
    case Target::AllEnemies:
        for (int i = 0; i < static_cast<int>(enemies.size()); i++)
        {
            if (IsEnemyAlive(i))
            {
                targets.push_back(&enemies[i]);
            }
        }
        break;
    }
    return targets;
}

// Play card
bool Battle::PlayCard(int handIndex, int targetIndex)
{
    if (!CanPlayCard(handIndex, targetIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);

    // Pay energy
    energy -= card.cost;

    // Apply effects
    ApplyCardEffects(card, {GetTargets(card, targetIndex), playerBlock});

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

    if (result != BattleResult::InProgress)
    {
        return;
    }

    // Next turn
    turn++;
    ChooseEnemyIntents();
    StartPlayerTurn();
}

// Next intents
void Battle::ChooseEnemyIntents()
{
    for (Enemy &enemy : enemies)
    {
        if (enemy.hp > 0)
        {
            enemy.intent = ChooseIntent(enemy, turn);
        }
    }
}

// Execute intents
void Battle::ExecuteEnemyTurn()
{
    for (Enemy &enemy : enemies)
    {
        if (enemy.hp <= 0)
        {
            continue;
        }

        // Expire block
        enemy.block = 0;

        // Boss action used
        if (enemy.intent.bossAction)
        {
            enemy.usedBossAction = true;
        }

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

        // Stop when player dies
        CheckResult();
        if (result != BattleResult::InProgress)
        {
            return;
        }
    }
}

// Check result
void Battle::CheckResult()
{
    if (player.hp <= 0)
    {
        result = BattleResult::Lost;
        return;
    }

    for (const Enemy &enemy : enemies)
    {
        if (enemy.hp > 0)
        {
            return;
        }
    }
    result = BattleResult::Won;
}
