#include <raylib.h>
#include "game/assets/assets.h"
#include "game/gameConfig.h"
#include "game/scenes/scenes.h"
#include "game/state/scene.h"
#include "game/ui/image.h"
#include "game/ui/style.h"
#include "game/ui/text.h"
#include "game/ui/widget.h"

static Rectangle GetPanel()
{
    return CenteredRect(560.0f, 480.0f, 300.0f);
}

static Rectangle GetPlayButton()
{
    return CenteredRect(600.0f, 336.0f, buttonHeight);
}

static Rectangle GetExitButton()
{
    return CenteredRect(720.0f, 336.0f, buttonHeight);
}

void UpdateMenuScene(ecs::Registry &registry)
{
    if (IsConfirmPressed(GetPlayButton()))
    {
        RequestScene(registry, Scene::CharacterSelect);
    }
    else if (IsClicked(GetExitButton()))
    {
        registry.Resource<SceneState>().quit = true;
    }
}

void DrawMenuScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    DrawSceneBackdrop(assets, assets.battleBackground, 120);
    DrawTextOutlined(assets.font, "KURIN", {screenWidth / 2.0f, 340.0f}, logoText, goldColor);
    DrawTextOutlined(assets.font, "A dungeon deckbuilder", {screenWidth / 2.0f, 460.0f}, bigText, peachColor);
    DrawPanel(assets, GetPanel());
    DrawButton(assets, GetPlayButton(), "Play", true);
    DrawButton(assets, GetExitButton(), "Exit", false);
}
