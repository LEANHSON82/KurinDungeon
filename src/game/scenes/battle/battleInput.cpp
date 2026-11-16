#include <raylib.h>
#include "core/components/cards.h"
#include "core/state/battle.h"
#include "core/state/run.h"
#include "core/systems/battle/combatRules.h"
#include "game/assets/assets.h"
#include "game/components/visual.h"
#include "game/scenes/battle/battleLayout.h"
#include "game/scenes/scenes.h"
#include "game/state/battleView.h"
#include "game/ui/widget.h"

static ecs::Entity NextLivingEnemy(const ecs::Registry &registry, ecs::Entity from, int step)
{
    std::vector<ecs::Entity> enemies = GetEnemiesInOrder(registry);
    int count = static_cast<int>(enemies.size());
    int index = -1;
    for (int i = 0; i < count; i++)
    {
        if (enemies[i] == from)
        {
            index = i;
        }
    }

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
        if (IsEnemyAlive(registry, enemies[index]))
        {
            return enemies[index];
        }
    }
    return from;
}

static void ToggleCard(BattleUi &ui, ecs::Entity card)
{
    ui.selectedCard = card == ui.selectedCard ? ecs::nullEntity : card;
}

static void PlaySelectedCard(ecs::Registry &registry, ecs::Entity target)
{
    BattleUi &ui = registry.Resource<BattleUi>();
    if (!CanPlayCard(registry, ui.selectedCard, target))
    {
        TraceLog(LOG_INFO, "PlayCard rejected by input");
        return;
    }

    PushBattleCommand(registry, {BattleCommandType::PlayCard, ui.selectedCard, target});
    PlayGameSound(registry.Resource<Assets>().playCard);
    ui.selectedCard = ecs::nullEntity;
}

static void ClickBattlefield(ecs::Registry &registry)
{
    ecs::Entity enemy = GetEnemyUnderMouse(registry);

    switch (GetSelectedCard(registry)->target)
    {
    case Target::SingleEnemy:
        if (!enemy.IsNull())
        {
            PlaySelectedCard(registry, enemy);
        }
        break;
    case Target::AllEnemies:
        if (!enemy.IsNull())
        {
            PlaySelectedCard(registry, ecs::nullEntity);
        }
        break;
    case Target::Self:
        if (IsHovered(GetActorRect(registry.Get<Position>(GetPlayer(registry)).value)))
        {
            PlaySelectedCard(registry, ecs::nullEntity);
        }
        break;
    }
}

static void UpdateKeyboard(ecs::Registry &registry)
{
    BattleUi &ui = registry.Resource<BattleUi>();
    if (IsKeyPressed(KEY_RIGHT))
    {
        ui.target = NextLivingEnemy(registry, ui.target, 1);
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        ui.target = NextLivingEnemy(registry, ui.target, -1);
    }

    std::vector<ecs::Entity> hand = GetPile(registry, Zone::Hand);
    for (int i = 0; i < static_cast<int>(hand.size()); i++)
    {
        if (IsKeyPressed(KEY_ONE + i))
        {
            ToggleCard(ui, hand[i]);
        }
    }

    if (IsKeyPressed(KEY_ENTER) && !ui.selectedCard.IsNull())
    {
        bool single = GetSelectedCard(registry)->target == Target::SingleEnemy;
        PlaySelectedCard(registry, single ? ui.target : ecs::nullEntity);
    }
}

static void UpdateMouse(ecs::Registry &registry)
{
    BattleUi &ui = registry.Resource<BattleUi>();
    ecs::Entity enemy = GetEnemyUnderMouse(registry);
    if (!enemy.IsNull())
    {
        ui.target = enemy;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return;
    }

    ecs::Entity card = GetCardUnderMouse(GetPile(registry, Zone::Hand), ui.selectedCard);
    if (!card.IsNull())
    {
        ToggleCard(ui, card);
    }
    else if (!ui.selectedCard.IsNull())
    {
        ClickBattlefield(registry);
    }
}

void BattleInputSystem(ecs::Registry &registry)
{
    if (registry.Resource<BattleState>().result != BattleResult::InProgress)
    {
        if (IsKeyPressed(KEY_ENTER) || IsClicked(GetContinueButton()))
        {
            PushRunCommand(registry, {RunCommandType::FinishBattle, ecs::nullEntity, 0, 0});
        }
        return;
    }

    BattleUi &ui = registry.Resource<BattleUi>();
    if (!IsEnemyAlive(registry, ui.target))
    {
        ui.target = NextLivingEnemy(registry, ui.target, 1);
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        ui.selectedCard = ecs::nullEntity;
        return;
    }

    if (IsKeyPressed(KEY_E) || IsClicked(GetEndTurnButton()))
    {
        PushBattleCommand(registry, {BattleCommandType::EndTurn, ecs::nullEntity, ecs::nullEntity});
        ui.selectedCard = ecs::nullEntity;
        return;
    }

    UpdateKeyboard(registry);
    UpdateMouse(registry);
}
