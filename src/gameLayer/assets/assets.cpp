#include "assets.h"
#include "cards/cardDatabase.h"
#include "relics/relicDatabase.h"

static const char *ResourcePath(const char *file)
{
    return TextFormat("%sresources/%s", GetApplicationDirectory(), file);
}

static Texture2D LoadGameTexture(Assets &assets, const char *file)
{
    Texture2D texture = LoadTexture(ResourcePath(file));
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    assets.textures.push_back(texture);
    return texture;
}

static Texture2D LoadSmoothTexture(Assets &assets, const char *file)
{
    if (!FileExists(ResourcePath(file)))
    {
        return Texture2D{};
    }
    Texture2D texture = LoadTexture(ResourcePath(file));
    GenTextureMipmaps(&texture);
    SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    assets.textures.push_back(texture);
    return texture;
}

static BoxImage LoadBox(Assets &assets, const char *file, Rectangle part, float cornerX, float cornerY)
{
    BoxImage box;
    box.texture = LoadGameTexture(assets, file);
    box.part = part;
    box.cornerX = cornerX;
    box.cornerY = cornerY;
    return box;
}

static Texture2D LoadSilhouette(Assets &assets, const char *file)
{
    Image image = LoadImage(ResourcePath(file));
    ImageColorBrightness(&image, 255);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    assets.textures.push_back(texture);
    return texture;
}

static SpriteSheet LoadSheet(Assets &assets, const char *character, const char *animation)
{
    SpriteSheet sheet;
    sheet.texture = LoadGameTexture(assets, TextFormat("images/characters/%s/shadow/%s.png", character, animation));
    sheet.silhouette = LoadSilhouette(assets, TextFormat("images/characters/%s/%s.png", character, animation));
    sheet.frameCount = sheet.texture.width / spriteFrameSize;
    return sheet;
}

static CharacterSprites LoadCharacter(Assets &assets, const char *character, bool hasBlock)
{
    CharacterSprites sprites;
    sprites.idle = LoadSheet(assets, character, "idle");
    sprites.attack = LoadSheet(assets, character, "attack01");
    sprites.hurt = LoadSheet(assets, character, "hurt");
    sprites.death = LoadSheet(assets, character, "death");
    if (hasBlock)
    {
        sprites.block = LoadSheet(assets, character, "block");
    }
    return sprites;
}

static Font LoadPixelFont(const char *file, int size)
{
    if (!FileExists(ResourcePath(file)))
    {
        return GetFontDefault();
    }
    Font font = LoadFontEx(ResourcePath(file), size, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    return font;
}

static void UnloadPixelFont(Font &font)
{
    if (font.texture.id != GetFontDefault().texture.id)
    {
        UnloadFont(font);
    }
}

static Sound LoadOptionalSound(const char *file)
{
    const char *path = ResourcePath(file);
    if (!FileExists(path))
    {
        return Sound{};
    }
    return LoadSound(path);
}

static void UnloadOptionalSound(Sound &sound)
{
    if (IsSoundReady(sound))
    {
        UnloadSound(sound);
    }
}

void LoadAssets(Assets &assets)
{
    assets.font = LoadPixelFont("fonts/jersey10.ttf", mainFontSize);
    assets.titleFont = LoadPixelFont("fonts/fantasy_rpg_title.ttf", titleFontSize);
    assets.logo = LoadSmoothTexture(assets, "images/branding/kurin-logo.png");

    assets.dungeon = LoadGameTexture(assets, "images/backgrounds/dungeon.png");
    assets.graveyard = LoadGameTexture(assets, "images/backgrounds/graveyard.png");

    assets.knight = LoadCharacter(assets, "knight", true);
    assets.swordsman = LoadCharacter(assets, "swordsman", false);
    assets.slime = LoadCharacter(assets, "slime", false);
    assets.armoredOrc = LoadCharacter(assets, "armored_orc", true);
    assets.eliteOrc = LoadCharacter(assets, "elite_orc", false);
    assets.armoredAxeman = LoadCharacter(assets, "armored_axeman", false);
    assets.necromancer = LoadCharacter(assets, "necromancer", false);

    assets.heartIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_03.png");
    assets.shieldIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_10.png");
    assets.starIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_01.png");
    assets.swordIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_09.png");
    assets.weakIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_02.png");
    assets.vulnerableIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_31.png");
    for (int i = 0; i < 10; i++)
    {
        assets.digits[i] = LoadGameTexture(assets, TextFormat("images/icons/base/%03d_number_%d.png", 201 + i, i));
    }
    for (int i = 0; i < cardCount; i++)
    {
        assets.cardArt[i] = LoadGameTexture(assets, GetCard(static_cast<CardId>(i)).art);
    }
    for (int i = 0; i < relicCount; i++)
    {
        assets.relicArt[i] = LoadGameTexture(assets, GetRelic(static_cast<RelicId>(i)).art);
    }
    assets.goldIcon = LoadGameTexture(assets, "images/icons/base/271_coin_gold.png");

    assets.startRoom = LoadGameTexture(assets, "images/icons/base/276_home.png");
    assets.combatRoom = LoadGameTexture(assets, "images/icons/base/012_steel_sword.png");
    assets.eliteRoom = LoadGameTexture(assets, "images/icons/base/138_skull_small.png");
    assets.shopRoom = LoadGameTexture(assets, "images/icons/base/094_gold_pouch.png");
    assets.eventRoom = LoadGameTexture(assets, "images/icons/base/232_question_mark.png");
    assets.treasureRoom = LoadGameTexture(assets, "images/icons/base/107_chest_big.png");
    assets.restRoom = LoadGameTexture(assets, "images/icons/base/141_fire.png");
    assets.bossRoom = LoadGameTexture(assets, "images/icons/base/137_skull_big.png");
    assets.mapHero = LoadGameTexture(assets, "images/icons/base/060_steel_helmet.png");

    assets.darkBox = LoadBox(assets, "images/ui/background_boxes/BGbox_01B.png", {6, 6, 36, 36}, 10, 10);
    assets.buttonNormal = LoadBox(assets, "images/ui/buttons/Button_01A_Normal.png", {0, 0, 48, 16}, 5, 7);
    assets.buttonHover = LoadBox(assets, "images/ui/buttons/Button_01A_Selected.png", {0, 0, 48, 16}, 5, 7);
    assets.buttonPressed = LoadBox(assets, "images/ui/buttons/Button_01A_Pressed.png", {0, 0, 48, 16}, 5, 7);
    assets.goldButtonNormal = LoadBox(assets, "images/ui/buttons/Button_03A_Normal.png", {0, 0, 48, 16}, 9, 7);
    assets.goldButtonHover = LoadBox(assets, "images/ui/buttons/Button_03A_Selected.png", {0, 0, 48, 16}, 9, 7);
    assets.goldButtonPressed = LoadBox(assets, "images/ui/buttons/Button_03A_Pressed.png", {0, 0, 48, 16}, 9, 7);
    assets.healthFrame = LoadBox(assets, "images/ui/sliders_bars/Slider01_Box.png", {1, 4, 46, 9}, 4, 4);
    assets.intentBanner = LoadBox(assets, "images/ui/title_banners/BannerSmall_01A.png", {0, 2, 48, 13}, 8, 6);
    assets.victoryRibbon = LoadBox(assets, "images/ui/title_banners/BannerMedium_02C.png", {2, 8, 44, 17}, 14, 8);
    assets.defeatRibbon = LoadBox(assets, "images/ui/title_banners/BannerMedium_02B.png", {2, 8, 44, 17}, 14, 8);
    assets.mapPaper = LoadBox(assets, "images/ui/background_boxes/BGbox_04C.png", {3, 4, 42, 40}, 20, 19);
    assets.healthFill = LoadGameTexture(assets, "images/ui/sliders_bars/Slider01_Bar02.png");
    assets.healthOrbFrame = LoadGameTexture(assets, "images/ui/resource_orbs/DecoratedFrame1_Left.png");
    assets.energyOrbFrame = LoadGameTexture(assets, "images/ui/resource_orbs/DecoratedFrame2_Right.png");
    assets.orbEmpty = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_Frame_Empty.png");
    assets.orbHealth = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_HP.png");
    assets.orbEnergy = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_Energy.png");

    assets.playCard = LoadOptionalSound("sounds/play_card.wav");
    assets.hit = LoadOptionalSound("sounds/hit.wav");
    assets.block = LoadOptionalSound("sounds/block.wav");
    assets.victory = LoadOptionalSound("sounds/victory.wav");
    assets.defeat = LoadOptionalSound("sounds/defeat.wav");
}

void UnloadAssets(Assets &assets)
{
    for (Texture2D texture : assets.textures)
    {
        if (texture.id != 0)
        {
            UnloadTexture(texture);
        }
    }
    assets.textures.clear();

    UnloadPixelFont(assets.font);
    UnloadPixelFont(assets.titleFont);

    UnloadOptionalSound(assets.playCard);
    UnloadOptionalSound(assets.hit);
    UnloadOptionalSound(assets.block);
    UnloadOptionalSound(assets.victory);
    UnloadOptionalSound(assets.defeat);
}

const CharacterSprites &GetHeroSprites(const Assets &assets, CharacterId id)
{
    switch (id)
    {
    case CharacterId::Knight:
        return assets.knight;
    case CharacterId::Swordsman:
        return assets.swordsman;
    case CharacterId::Count:
        break;
    }
    return assets.knight;
}

const CharacterSprites &GetEnemySprites(const Assets &assets, EnemyId id)
{
    switch (id)
    {
    case EnemyId::Slime:
        return assets.slime;
    case EnemyId::Guardian:
        return assets.armoredOrc;
    case EnemyId::Brute:
        return assets.eliteOrc;
    case EnemyId::Axeman:
        return assets.armoredAxeman;
    case EnemyId::Warden:
        return assets.necromancer;
    case EnemyId::Count:
        break;
    }
    return assets.slime;
}

const SpriteSheet &GetAnimation(const CharacterSprites &sprites, AnimationType type)
{
    const SpriteSheet *sheet = &sprites.idle;
    switch (type)
    {
    case AnimationType::Idle:
        break;
    case AnimationType::Attack:
        sheet = &sprites.attack;
        break;
    case AnimationType::Block:
        sheet = &sprites.block;
        break;
    case AnimationType::Hurt:
        sheet = &sprites.hurt;
        break;
    case AnimationType::Death:
        sheet = &sprites.death;
        break;
    }
    return sheet->frameCount > 0 ? *sheet : sprites.idle;
}

Texture2D GetStatusIcon(const Assets &assets, StatusType type)
{
    switch (type)
    {
    case StatusType::Strength:
        return assets.starIcon;
    case StatusType::Weak:
        return assets.weakIcon;
    case StatusType::Vulnerable:
        return assets.vulnerableIcon;
    case StatusType::Count:
        break;
    }
    return assets.starIcon;
}

Texture2D GetRoomIcon(const Assets &assets, RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return assets.startRoom;
    case RoomType::Combat:
        return assets.combatRoom;
    case RoomType::Elite:
        return assets.eliteRoom;
    case RoomType::Rest:
        return assets.restRoom;
    case RoomType::Shop:
        return assets.shopRoom;
    case RoomType::Event:
        return assets.eventRoom;
    case RoomType::Treasure:
        return assets.treasureRoom;
    case RoomType::Boss:
        return assets.bossRoom;
    }
    return assets.combatRoom;
}

void PlayGameSound(const Sound &sound)
{
    if (IsSoundReady(sound))
    {
        PlaySound(sound);
    }
}
