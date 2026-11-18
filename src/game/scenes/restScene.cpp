#include <raylib.h>
#include "core/components/combat.h"
#include "core/state/run.h"
#include "game/assets/assets.h"
#include "game/gameConfig.h"
#include "game/systems/animationSystem.h"
#include "game/scenes/scenes.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static Rectangle GetPanel()
{
    return CenteredRect(220.0f, 720.0f, 520.0f);
}

static Rectangle GetRestButton()
{
    return CenteredRect(800.0f, 384.0f, buttonHeight);
}

void UpdateRestScene(ecs::Registry &registry)
{
    if (IsConfirmPressed(GetRestButton()))
    {
        PushRunCommand(registry, TakeRestCommand());
    }
}

void DrawRestScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    const Health &health = registry.Get<Health>(registry.Resource<RunState>().player);

    DrawSceneBackdrop(assets, assets.mapBackground, 120, "Rest");

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    DrawSprite(idle, GetLoopFrame(idle, GetTime()), {screenWidth / 2.0f - 110.0f, panel.y + 320.0f}, false, WHITE);
    DrawIcon(assets.restRoom, {screenWidth / 2.0f + 110.0f, panel.y + 270.0f}, uiScale, WHITE);

    DrawIcon(assets.heartIcon, {screenWidth / 2.0f - 90.0f, panel.y + 430.0f}, uiScale, WHITE);
    DrawTextLeft(assets.font, TextFormat("%d / %d", health.hp, health.maxHp), {screenWidth / 2.0f - 50.0f, panel.y + 398.0f}, bigText, textColor);

    DrawButton(assets, GetRestButton(), TextFormat("Heal %d HP", restHeal), true);
}
