#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include <raylib.h>
#include "core/data/cards.h"
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

static std::string DescribeHero()
{
    std::vector<CardId> shown;
    std::string text = std::to_string(startingHp) + " HP - ";
    for (CardId id : GetStartingDeck())
    {
        if (std::find(shown.begin(), shown.end(), id) != shown.end())
        {
            continue;
        }
        if (!shown.empty())
        {
            text += ", ";
        }
        text += GetCard(id).name;
        shown.push_back(id);
    }
    return text;
}

void UpdateCharacterSelectScene(ecs::Registry &registry)
{
    if (IsConfirmPressed(GetConfirmButton()))
    {
        PushRunCommand(registry, StartRunCommand(std::random_device{}()));
    }
}

void DrawCharacterSelectScene(const ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();
    DrawSceneBackdrop(assets, assets.battleBackground, 120, "Choose your hero");

    Rectangle panel = GetPanel();
    DrawPanel(assets, panel);
    const SpriteSheet &idle = assets.knight.idle;
    DrawSprite(idle, GetLoopFrame(idle, GetTime()), {screenWidth / 2.0f, panel.y + 330.0f}, false, WHITE);

    DrawTextOutlined(assets.font, "Knight", {screenWidth / 2.0f, panel.y + 400.0f}, titleText, goldColor);
    DrawTextCenter(assets.font, DescribeHero().c_str(), {screenWidth / 2.0f, panel.y + 470.0f}, smallText, peachColor);

    DrawButton(assets, GetConfirmButton(), "Confirm", true);
}
