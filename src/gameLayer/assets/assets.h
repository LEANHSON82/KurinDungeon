#pragma once
#include <raylib.h>
#include <vector>
#include "cards/card.h"
#include "characters/character.h"
#include "combat/enemies/enemy.h"
#include "combat/status.h"
#include "map/dungeonMap.h"
#include "relics/relic.h"

const int mainFontSize = 20;
const int titleFontSize = 13;
const int spriteFrameSize = 100;

enum class AnimationType
{
    Idle,
    Attack,
    Block,
    Hurt,
    Death,
};

struct SpriteSheet
{
    Texture2D texture{};
    Texture2D silhouette{};
    int frameCount = 0;
};

struct BoxImage
{
    Texture2D texture{};
    Rectangle part{};
    float cornerX = 0.0f;
    float cornerY = 0.0f;
};

struct CharacterSprites
{
    SpriteSheet idle;
    SpriteSheet attack;
    SpriteSheet block;
    SpriteSheet hurt;
    SpriteSheet death;
};

struct Assets
{
    Font font{};
    Font titleFont{};
    Texture2D logo{};

    Texture2D dungeon{};
    Texture2D graveyard{};

    CharacterSprites knight;
    CharacterSprites swordsman;
    CharacterSprites slime;
    CharacterSprites armoredOrc;
    CharacterSprites eliteOrc;
    CharacterSprites armoredAxeman;
    CharacterSprites necromancer;

    Texture2D heartIcon{};
    Texture2D shieldIcon{};
    Texture2D starIcon{};
    Texture2D swordIcon{};
    Texture2D weakIcon{};
    Texture2D vulnerableIcon{};
    Texture2D digits[10]{};
    Texture2D cardArt[cardCount]{};
    Texture2D relicArt[relicCount]{};
    Texture2D goldIcon{};

    Texture2D startRoom{};
    Texture2D combatRoom{};
    Texture2D eliteRoom{};
    Texture2D shopRoom{};
    Texture2D eventRoom{};
    Texture2D treasureRoom{};
    Texture2D restRoom{};
    Texture2D bossRoom{};
    Texture2D mapHero{};

    BoxImage darkBox;
    BoxImage buttonNormal;
    BoxImage buttonHover;
    BoxImage buttonPressed;
    BoxImage goldButtonNormal;
    BoxImage goldButtonHover;
    BoxImage goldButtonPressed;
    BoxImage healthFrame;
    BoxImage intentBanner;
    BoxImage victoryRibbon;
    BoxImage defeatRibbon;
    BoxImage mapPaper;
    Texture2D healthFill{};
    Texture2D healthOrbFrame{};
    Texture2D energyOrbFrame{};
    Texture2D orbEmpty{};
    Texture2D orbHealth{};
    Texture2D orbEnergy{};

    Sound playCard{};
    Sound hit{};
    Sound block{};
    Sound victory{};
    Sound defeat{};

    std::vector<Texture2D> textures;
};

void LoadAssets(Assets &assets);
void UnloadAssets(Assets &assets);

const CharacterSprites &GetHeroSprites(const Assets &assets, CharacterId id);
const CharacterSprites &GetEnemySprites(const Assets &assets, EnemyId id);
const SpriteSheet &GetAnimation(const CharacterSprites &sprites, AnimationType type);
Texture2D GetStatusIcon(const Assets &assets, StatusType type);
Texture2D GetRoomIcon(const Assets &assets, RoomType type);

void PlayGameSound(const Sound &sound);
