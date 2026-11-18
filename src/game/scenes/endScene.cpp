#include <string>
#include <raylib.h>
#include "core/data/enemies.h"
#include "game/assets/assets.h"
#include "game/gameConfig.h"
#include "game/scenes/scenes.h"
#include "game/state/scene.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static Rectangle GetMenuButton()
{
    return CenteredRect(640.0f, 432.0f, buttonHeight);
}

void UpdateEndScene(ecs::Registry &registry)
{
    if (IsConfirmPressed(GetMenuButton()))
    {
        RequestScene(registry, Scene::Menu);
    }
}

void DrawEndScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    bool won = registry.Resource<SceneState>().current == Scene::Victory;

    DrawSceneBackdrop(assets, assets.battleBackground, 150);

    Rectangle ribbon = CenteredRect(330.0f, 840.0f, 17.0f * uiScale);
    DrawBox(won ? assets.victoryRibbon : assets.defeatRibbon, ribbon, WHITE);
    DrawTextOutlined(assets.font, won ? "Victory" : "Game Over", {screenWidth / 2.0f, ribbon.y + ribbon.height / 2.0f - titleTextNudge}, titleText, textColor);
    std::string message = won ? std::string("The ") + GetEnemyDefinition(bossEnemy).name + " has fallen." : "Your run ends here.";
    DrawTextCenter(assets.font, message.c_str(), {screenWidth / 2.0f, 540.0f}, bigText, peachColor);

    DrawButton(assets, GetMenuButton(), "Back to menu", true);
}
