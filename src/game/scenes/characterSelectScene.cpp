#include <random>
#include <raylib.h>
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
    return CenteredRect(220.0f, 720.0f, 560.0f);
}

static Rectangle GetConfirmButton()
{
    return CenteredRect(840.0f, 336.0f, buttonHeight);
}

void UpdateCharacterSelectScene(ecs::Registry &registry)
{
    if (IsKeyPressed(KEY_ENTER) || IsClicked(GetConfirmButton()))
    {
        PushRunCommand(registry, {RunCommandType::StartRun, ecs::nullEntity, 0, std::random_device{}()});
    }
}

void DrawCharacterSelectScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    DrawBackground(assets.battleBackground, WHITE);
    DrawScreenShade(120);
    DrawTextOutlined(assets.font, "Choose your hero", {screenWidth / 2.0f, headerY}, titleText, goldColor);

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    int frame = static_cast<int>(GetTime() / animationFrameTime) % idle.frameCount;
    DrawSprite(idle, frame, {screenWidth / 2.0f, panel.y + 330.0f}, false, WHITE);

    DrawTextOutlined(assets.font, "Knight", {screenWidth / 2.0f, panel.y + 400.0f}, titleText, goldColor);
    DrawTextCenter(assets.font, "80 HP - Strike, Defend, Heavy Strike", {screenWidth / 2.0f, panel.y + 470.0f}, smallText, peachColor);

    DrawButton(assets, GetConfirmButton(), "Confirm", true);
}
