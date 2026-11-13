#pragma once
#include <raylib.h>
#include <vector>
#include "cards/card.h"
#include "combat/enemies/enemy.h"
#include "map/dungeonMap.h"

const int fontSize = 20;

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
    Texture2D logo{};

    Texture2D battleBackground{};
    Texture2D mapBackground{};

    CharacterSprites knight;
    CharacterSprites slime;
    CharacterSprites armoredOrc;
    CharacterSprites necromancer;

    Texture2D heartIcon{};
    Texture2D shieldIcon{};
    Texture2D starIcon{};
    Texture2D swordIcon{};
    Texture2D skullIcon{};
    Texture2D digits[10]{};

    Texture2D strikeArt{};
    Texture2D defendArt{};
    Texture2D heavyStrikeArt{};
    Texture2D cleaveArt{};

    Texture2D startRoom{};
    Texture2D restRoom{};
    Texture2D openSlot{};
    Texture2D lockedSlot{};
    Texture2D currentSlot{};

    BoxImage darkBox;
    BoxImage buttonNormal;
    BoxImage buttonHover;
    BoxImage buttonPressed;
    BoxImage goldButtonNormal;
    BoxImage goldButtonHover;
    BoxImage goldButtonPressed;
    BoxImage healthFrame;
    Texture2D healthFill{};
    BoxImage intentBanner;
    BoxImage victoryRibbon;
    BoxImage defeatRibbon;
    Texture2D healthOrbFrame{};
    Texture2D energyOrbFrame{};
    Texture2D orbEmpty{};
    Texture2D orbHealth{};
    Texture2D orbEnergy{};
    BoxImage mapPaper;

    Sound playCard{};
    Sound hit{};
    Sound block{};
    Sound victory{};
    Sound defeat{};

    std::vector<Texture2D> textures;
};

void LoadAssets(Assets &assets);
void UnloadAssets(Assets &assets);

const CharacterSprites &GetEnemySprites(const Assets &assets, EnemyId id);
const SpriteSheet &GetAnimation(const CharacterSprites &sprites, AnimationType type);
Texture2D GetCardArt(const Assets &assets, CardId id);
Texture2D GetIntentIcon(const Assets &assets, IntentType type);
Texture2D GetRoomIcon(const Assets &assets, RoomType type);

void PlayGameSound(const Sound &sound);
