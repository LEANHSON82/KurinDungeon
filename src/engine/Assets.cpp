#include "engine/Assets.h"
#include <algorithm>
#include <fstream>
#include <sstream>

// Every character sheet in the asset pack uses 100 x 100 pixel frames.
static constexpr int spriteFrameWidth = 100;

template <typename T>
static const T &Find(const std::map<std::string, T> &items, const std::string &name, const char *kind)
{
    static const T empty{};
    auto found = items.find(name);
    if (found == items.end())
    {
        TraceLog(LOG_WARNING, "Unknown %s: %s", kind, name.c_str());
        return empty;
    }
    return found->second;
}

std::string Assets::path(const std::string &file) const
{
    return m_folder + file;
}

bool Assets::checkFile(const std::string &fullPath)
{
    if (FileExists(fullPath.c_str()))
    {
        return true;
    }
    TraceLog(LOG_WARNING, "Missing asset: %s", fullPath.c_str());
    m_missingFiles++;
    return false;
}

Texture2D Assets::loadTexture(const std::string &file, int filter)
{
    std::string fullPath = path(file);
    if (!checkFile(fullPath))
    {
        return Texture2D{};
    }
    Texture2D texture = LoadTexture(fullPath.c_str());
    if (filter == TEXTURE_FILTER_TRILINEAR)
    {
        GenTextureMipmaps(&texture);
    }
    SetTextureFilter(texture, filter);
    return texture;
}

// A white copy of a sprite sheet, tinted to draw outlines around characters.
Texture2D Assets::loadSilhouette(const std::string &file)
{
    std::string fullPath = path(file);
    if (!checkFile(fullPath))
    {
        return Texture2D{};
    }
    Image image = LoadImage(fullPath.c_str());
    ImageColorBrightness(&image, 255);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    return texture;
}

void Assets::addTexture(const std::string &name, const std::string &file, int filter)
{
    m_textures[name] = loadTexture(file, filter);
}

void Assets::addBox(const std::string &name, const std::string &file, Rectangle part, float cornerX, float cornerY)
{
    m_boxes[name] = BoxImage{loadTexture(file, TEXTURE_FILTER_POINT), part, cornerX, cornerY};
}

void Assets::addAnimation(const std::string &name, const std::string &sheet, const std::string &plainSheet, int speed)
{
    Texture2D texture = loadTexture(sheet, TEXTURE_FILTER_POINT);
    Texture2D silhouette = loadSilhouette(plainSheet);
    int frameCount = std::max(1, texture.width / spriteFrameWidth);
    m_animations[name] = Animation(name, texture, silhouette, static_cast<std::size_t>(frameCount), static_cast<std::size_t>(speed));
}

void Assets::addFont(const std::string &name, const std::string &file, int size)
{
    std::string fullPath = path(file);
    if (!checkFile(fullPath))
    {
        m_fonts[name] = GetFontDefault();
        return;
    }
    Font font = LoadFontEx(fullPath.c_str(), size, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    m_fonts[name] = font;
}

void Assets::addSound(const std::string &name, const std::string &file)
{
    std::string fullPath = path(file);
    m_sounds[name] = checkFile(fullPath) ? LoadSound(fullPath.c_str()) : Sound{};
}

// One asset per line:
//   Texture   Name file [smooth]
//   Box       Name file x y width height cornerX cornerY
//   Animation Name sheet plainSheet speed
//   Font      Name file size
//   Sound     Name file
void Assets::loadFromFile(const std::string &folder, const std::string &listFile)
{
    m_folder = folder;
    std::ifstream file(path(listFile));
    if (!file)
    {
        TraceLog(LOG_ERROR, "Cannot open asset list: %s", path(listFile).c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream words(line);
        std::string type;
        std::string name;
        if (!(words >> type) || type[0] == '#')
        {
            continue;
        }
        words >> name;

        if (type == "Texture")
        {
            std::string texturePath;
            std::string filter;
            words >> texturePath >> filter;
            addTexture(name, texturePath, filter == "smooth" ? TEXTURE_FILTER_TRILINEAR : TEXTURE_FILTER_POINT);
        }
        else if (type == "Box")
        {
            std::string boxPath;
            Rectangle part{};
            float cornerX = 0.0f;
            float cornerY = 0.0f;
            words >> boxPath >> part.x >> part.y >> part.width >> part.height >> cornerX >> cornerY;
            addBox(name, boxPath, part, cornerX, cornerY);
        }
        else if (type == "Animation")
        {
            std::string sheet;
            std::string plainSheet;
            int speed = 1;
            words >> sheet >> plainSheet >> speed;
            addAnimation(name, sheet, plainSheet, speed);
        }
        else if (type == "Font")
        {
            std::string fontPath;
            int size = 20;
            words >> fontPath >> size;
            addFont(name, fontPath, size);
        }
        else if (type == "Sound")
        {
            std::string soundPath;
            words >> soundPath;
            addSound(name, soundPath);
        }
        else
        {
            TraceLog(LOG_WARNING, "Unknown asset type '%s' in %s", type.c_str(), listFile.c_str());
        }
    }

    if (m_missingFiles > 0)
    {
        TraceLog(LOG_WARNING, "%d asset files are missing. Copy the purchased assets into resources/ (see .gitignore).", m_missingFiles);
    }
}

void Assets::unload()
{
    auto unloadTexture = [](Texture2D texture) {
        if (texture.id != 0)
        {
            UnloadTexture(texture);
        }
    };
    for (auto &[name, texture] : m_textures)
    {
        unloadTexture(texture);
    }
    for (auto &[name, box] : m_boxes)
    {
        unloadTexture(box.texture);
    }
    for (auto &[name, animation] : m_animations)
    {
        unloadTexture(animation.getTexture());
        unloadTexture(animation.getSilhouette());
    }
    for (auto &[name, font] : m_fonts)
    {
        if (font.texture.id != GetFontDefault().texture.id)
        {
            UnloadFont(font);
        }
    }
    for (auto &[name, sound] : m_sounds)
    {
        if (IsSoundReady(sound))
        {
            UnloadSound(sound);
        }
    }
    m_textures.clear();
    m_boxes.clear();
    m_animations.clear();
    m_fonts.clear();
    m_sounds.clear();
}

const Texture2D &Assets::getTexture(const std::string &name) const
{
    return Find(m_textures, name, "texture");
}

const BoxImage &Assets::getBox(const std::string &name) const
{
    return Find(m_boxes, name, "box");
}

const Animation &Assets::getAnimation(const std::string &name) const
{
    return Find(m_animations, name, "animation");
}

bool Assets::hasAnimation(const std::string &name) const
{
    return m_animations.count(name) > 0;
}

const Font &Assets::getFont(const std::string &name) const
{
    auto found = m_fonts.find(name);
    if (found != m_fonts.end())
    {
        return found->second;
    }
    TraceLog(LOG_WARNING, "Unknown font: %s", name.c_str());
    static const Font fallback = GetFontDefault();
    return fallback;
}

const Sound &Assets::getSound(const std::string &name) const
{
    return Find(m_sounds, name, "sound");
}

void Assets::playSound(const std::string &name) const
{
    const Sound &sound = getSound(name);
    if (IsSoundReady(sound))
    {
        PlaySound(sound);
    }
}
