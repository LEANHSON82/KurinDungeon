#include "core/systems/battle/combatRules.h"
#include "game/assets/assets.h"
#include "game/components/visual.h"
#include "game/prefabs/viewPrefabs.h"
#include "game/scenes/battle/battleLayout.h"
#include "game/scenes/scenes.h"
#include "game/state/battleView.h"
#include "game/systems/playbackSystem.h"

void SetupBattleScene(ecs::Registry &registry)
{
    const Assets &assets = registry.Resource<Assets>();

    AttachActorVisuals(registry, GetPlayer(registry), assets.knight, GetPlayerFeet(), false);

    std::vector<ecs::Entity> enemies = GetEnemiesInOrder(registry);
    int count = static_cast<int>(enemies.size());
    for (ecs::Entity entity : enemies)
    {
        const Enemy &enemy = registry.Get<Enemy>(entity);
        AttachActorVisuals(registry, entity, GetEnemySprites(assets, enemy.id), GetEnemyFeet(enemy.slot, count), true);
    }

    BattleUi &ui = registry.Resource<BattleUi>();
    ui = BattleUi{};
    ui.target = enemies.empty() ? ecs::nullEntity : enemies.front();
    registry.Resource<Playback>() = Playback{};
}

void UpdateBattleScene(ecs::Registry &registry, float deltaTime)
{
    PlaybackSystem(registry, deltaTime);
    if (!IsPlayingBack(registry))
    {
        BattleInputSystem(registry);
    }
}

void TeardownBattleScene(ecs::Registry &registry)
{
    // Only strip the visuals this scene attached to actors.
    for (ecs::Entity actor : registry.View<Sprite>().Collect())
    {
        registry.Remove<Position>(actor);
        registry.Remove<Sprite>(actor);
        registry.Remove<Animator>(actor);
    }
    registry.Resource<BattleUi>() = BattleUi{};
}
