#include "battle.h"
#include "combat/effects/cardEffect.h"
#include "cards/cardDatabase.h"
#include "damage.h"
#include "combat/enemies/enemyAi.h"

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

void Battle::StartPlayerTurn()
{
    energy = startingEnergy;
    playerBlock = 0;
    deck.DrawCards(handSize);
}

bool Battle::IsEnemyAlive(int index) const
{
    return index >= 0 && index < static_cast<int>(enemies.size()) && enemies[index].hp > 0;
}

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

std::vector<int> Battle::GetTargets(const Card &card, int targetIndex) const
{
    std::vector<int> targets;
    switch (card.target)
    {
    case Target::Self:
        break;
    case Target::SingleEnemy:
        targets.push_back(targetIndex);
        break;
    case Target::AllEnemies:
        for (int i = 0; i < static_cast<int>(enemies.size()); i++)
        {
            if (IsEnemyAlive(i))
            {
                targets.push_back(i);
            }
        }
        break;
    }
    return targets;
}

bool Battle::PlayCard(int handIndex, int targetIndex)
{
    if (!CanPlayCard(handIndex, targetIndex))
    {
        return false;
    }

    const Card &card = GetCard(deck.GetHand()[handIndex]);

    energy -= card.cost;

    EffectContext context{enemies, GetTargets(card, targetIndex), playerBlock, events};
    ApplyCardEffects(card, context);

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

    if (result != BattleResult::InProgress)
    {
        return;
    }

    turn++;
    ChooseEnemyIntents();
    StartPlayerTurn();
}

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

void Battle::ExecuteEnemyTurn()
{
    for (int i = 0; i < static_cast<int>(enemies.size()); i++)
    {
        Enemy &enemy = enemies[i];
        if (enemy.hp <= 0)
        {
            continue;
        }

        enemy.block = 0;

        if (enemy.intent.bossAction)
        {
            enemy.usedBossAction = true;
        }

        switch (enemy.intent.type)
        {
        case IntentType::Attack:
        {
            int hpBefore = player.hp;
            ApplyDamage(enemy.intent.value, player.hp, playerBlock);
            AddEvent(BattleEventType::Damage, i, playerActor, hpBefore - player.hp);
            if (player.hp == 0)
            {
                AddEvent(BattleEventType::Death, i, playerActor, 0);
            }
            break;
        }
        case IntentType::Defend:
            enemy.block += enemy.intent.value;
            AddEvent(BattleEventType::Block, i, i, enemy.intent.value);
            break;
        case IntentType::Buff:
            enemy.strength += enemy.intent.value;
            AddEvent(BattleEventType::Buff, i, i, enemy.intent.value);
            break;
        }

        CheckResult();
        if (result != BattleResult::InProgress)
        {
            return;
        }
    }
}

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

void Battle::AddEvent(BattleEventType type, int source, int target, int value)
{
    events.push_back({type, source, target, value});
}

std::vector<BattleEvent> Battle::TakeEvents()
{
    std::vector<BattleEvent> taken;
    taken.swap(events);
    return taken;
}
