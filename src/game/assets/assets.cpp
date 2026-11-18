#include "game/assets/assets.h"
#include <algorithm>
#include <iterator>
#include <string>

static constexpr int fontSize = 20;
static constexpr int spriteFrameWidth = 100;

// Same order as CardId.
static const char *const cardArtFiles[]{
    "images/icons/spells/199_power_strike.png",
    "images/icons/spells/112_holy_shield.png",
    "images/icons/spells/208_finishing_blow.png",
    "images/icons/spells/200_whirlwind.png",
};
static_assert(std::size(cardArtFiles) == cardCount, "Every CardId needs card art");

struct EnemySpriteFiles
{
    const char *character;
    bool hasBlock;
};

// Same order as EnemyId.
static const EnemySpriteFiles enemySpriteFiles[]{
    {"slime", false},
    {"armored_orc", true},
    {"necromancer", false},
};
static_assert(std::size(enemySpriteFiles) == enemyCount, "Every EnemyId needs sprites");

static std::string ResourcePath(const std::string &file)
{
    return std::string(GetApplicationDirectory()) + "resources/" + file;
}

static bool CheckFile(Assets &assets, const std::string &path)
{
    if (FileExists(path.c_str()))
    {
        return true;
    }
    TraceLog(LOG_WARNING, "Missing asset: %s", path.c_str());
    assets.missingFiles++;
    return false;
}

static Texture2D LoadGameTexture(Assets &assets, const std::string &file, int filter = TEXTURE_FILTER_POINT)
{
    std::string path = ResourcePath(file);
    if (!CheckFile(assets, path))
    {
        return Texture2D{};
    }
    Texture2D texture = LoadTexture(path.c_str());
    if (filter == TEXTURE_FILTER_TRILINEAR)
    {
        GenTextureMipmaps(&texture);
    }
    SetTextureFilter(texture, filter);
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

static Texture2D LoadSilhouette(Assets &assets, const std::string &file)
{
    std::string path = ResourcePath(file);
    if (!CheckFile(assets, path))
    {
        return Texture2D{};
    }
    Image image = LoadImage(path.c_str());
    ImageColorBrightness(&image, 255);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    assets.textures.push_back(texture);
    return texture;
}

static SpriteSheet LoadSheet(Assets &assets, const char *character, const char *animation)
{
    std::string folder = std::string("images/characters/") + character + "/";
    SpriteSheet sheet;
    sheet.texture = LoadGameTexture(assets, folder + "shadow/" + animation + ".png");
    sheet.silhouette = LoadSilhouette(assets, folder + animation + ".png");
    // At least one frame, so a missing file never causes a division by zero.
    sheet.frameCount = std::max(1, sheet.texture.width / spriteFrameWidth);
    return sheet;
}

static CharacterSprites LoadCharacter(Assets &assets, const char *character, const char *attack, bool hasBlock)
{
    CharacterSprites sprites;
    sprites.idle = LoadSheet(assets, character, "idle");
    sprites.attack = LoadSheet(assets, character, attack);
    sprites.hurt = LoadSheet(assets, character, "hurt");
    sprites.death = LoadSheet(assets, character, "death");
    if (hasBlock)
    {
        sprites.block = LoadSheet(assets, character, "block");
    }
    return sprites;
}

static Font LoadPixelFont(Assets &assets, const char *file, int size)
{
    std::string path = ResourcePath(file);
    if (!CheckFile(assets, path))
    {
        return GetFontDefault();
    }
    Font font = LoadFontEx(path.c_str(), size, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    return font;
}

static Sound LoadOptionalSound(const char *file)
{
    std::string path = ResourcePath(file);
    if (!FileExists(path.c_str()))
    {
        return Sound{};
    }
    return LoadSound(path.c_str());
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
    assets.font = LoadPixelFont(assets, "fonts/jersey10.ttf", fontSize);
    // The logo is a large painting shown scaled down, not pixel art.
    assets.logo = LoadGameTexture(assets, "images/branding/kurin-logo.png", TEXTURE_FILTER_TRILINEAR);

    assets.battleBackground = LoadGameTexture(assets, "images/backgrounds/dungeon.png");
    assets.mapBackground = LoadGameTexture(assets, "images/backgrounds/graveyard.png");

    assets.knight = LoadCharacter(assets, "knight", "attack01", true);
    for (int i = 0; i < enemyCount; i++)
    {
        assets.enemySprites[i] = LoadCharacter(assets, enemySpriteFiles[i].character, "attack01", enemySpriteFiles[i].hasBlock);
    }

    assets.heartIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_03.png");
    assets.shieldIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_10.png");
    assets.starIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_01.png");
    assets.swordIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_09.png");
    assets.skullIcon = LoadGameTexture(assets, "images/ui/mini_icons/Icon_06.png");
    for (int i = 0; i < 10; i++)
    {
        assets.digits[i] = LoadGameTexture(assets, TextFormat("images/icons/base/%03d_number_%d.png", 201 + i, i));
    }
    for (int i = 0; i < cardCount; i++)
    {
        assets.cardArt[i] = LoadGameTexture(assets, cardArtFiles[i]);
    }

    assets.startRoom = LoadGameTexture(assets, "images/icons/base/276_home.png");
    assets.restRoom = LoadGameTexture(assets, "images/icons/base/141_fire.png");

    assets.darkBox = LoadBox(assets, "images/ui/background_boxes/BGbox_01B.png", {6, 6, 36, 36}, 10, 10);
    assets.buttonNormal = LoadBox(assets, "images/ui/buttons/Button_01A_Normal.png", {0, 0, 48, 16}, 5, 7);
    assets.buttonHover = LoadBox(assets, "images/ui/buttons/Button_01A_Selected.png", {0, 0, 48, 16}, 5, 7);
    assets.buttonPressed = LoadBox(assets, "images/ui/buttons/Button_01A_Pressed.png", {0, 0, 48, 16}, 5, 7);
    assets.goldButtonNormal = LoadBox(assets, "images/ui/buttons/Button_03A_Normal.png", {0, 0, 48, 16}, 9, 7);
    assets.goldButtonHover = LoadBox(assets, "images/ui/buttons/Button_03A_Selected.png", {0, 0, 48, 16}, 9, 7);
    assets.goldButtonPressed = LoadBox(assets, "images/ui/buttons/Button_03A_Pressed.png", {0, 0, 48, 16}, 9, 7);
    assets.healthFrame = LoadBox(assets, "images/ui/sliders_bars/Slider01_Box.png", {1, 4, 46, 9}, 4, 4);
    assets.healthFill = LoadGameTexture(assets, "images/ui/sliders_bars/Slider01_Bar02.png");
    assets.intentBanner = LoadBox(assets, "images/ui/title_banners/BannerSmall_01A.png", {0, 2, 48, 13}, 8, 6);
    assets.victoryRibbon = LoadBox(assets, "images/ui/title_banners/BannerMedium_02C.png", {2, 8, 44, 17}, 14, 8);
    assets.defeatRibbon = LoadBox(assets, "images/ui/title_banners/BannerMedium_02B.png", {2, 8, 44, 17}, 14, 8);
    assets.healthOrbFrame = LoadGameTexture(assets, "images/ui/resource_orbs/DecoratedFrame1_Left.png");
    assets.energyOrbFrame = LoadGameTexture(assets, "images/ui/resource_orbs/DecoratedFrame2_Right.png");
    assets.orbEmpty = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_Frame_Empty.png");
    assets.orbHealth = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_HP.png");
    assets.orbEnergy = LoadGameTexture(assets, "images/ui/resource_orbs/Orb_Energy.png");
    assets.mapPaper = LoadBox(assets, "images/ui/background_boxes/BGbox_04B.png", {3, 4, 42, 40}, 20, 19);

    assets.playCard = LoadOptionalSound("sounds/play_card.wav");
    assets.hit = LoadOptionalSound("sounds/hit.wav");
    assets.block = LoadOptionalSound("sounds/block.wav");
    assets.victory = LoadOptionalSound("sounds/victory.wav");
    assets.defeat = LoadOptionalSound("sounds/defeat.wav");

    if (assets.missingFiles > 0)
    {
        TraceLog(LOG_WARNING, "%d asset files are missing. Copy the purchased assets into resources/ (see .gitignore).", assets.missingFiles);
    }
}

void UnloadAssets(Assets &assets)
{
    for (Texture2D texture : assets.textures)
    {
        UnloadTexture(texture);
    }
    assets.textures.clear();

    UnloadFont(assets.font);

    UnloadOptionalSound(assets.playCard);
    UnloadOptionalSound(assets.hit);
    UnloadOptionalSound(assets.block);
    UnloadOptionalSound(assets.victory);
    UnloadOptionalSound(assets.defeat);
}

const CharacterSprites &GetEnemySprites(const Assets &assets, EnemyId id)
{
    return assets.enemySprites[static_cast<int>(id)];
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

Texture2D GetCardArt(const Assets &assets, CardId id)
{
    return assets.cardArt[static_cast<int>(id)];
}

Texture2D GetIntentIcon(const Assets &assets, IntentType type)
{
    switch (type)
    {
    case IntentType::Attack:
        return assets.swordIcon;
    case IntentType::Defend:
        return assets.shieldIcon;
    case IntentType::Buff:
        return assets.starIcon;
    }
    return assets.swordIcon;
}

Texture2D GetRoomIcon(const Assets &assets, RoomType type)
{
    switch (type)
    {
    case RoomType::Start:
        return assets.startRoom;
    case RoomType::Combat:
        return assets.swordIcon;
    case RoomType::Rest:
        return assets.restRoom;
    case RoomType::Boss:
        return assets.skullIcon;
    }
    return assets.swordIcon;
}

void PlayGameSound(const Sound &sound)
{
    if (IsSoundReady(sound))
    {
        PlaySound(sound);
    }
}
