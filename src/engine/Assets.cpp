#include "engine/Assets.h"
#include <algorithm>
#include <fstream>
#include <sstream>

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

// Warns about a line of the asset list that has too few columns or a number that is not one.
static void CheckColumns(const std::istringstream &words, const std::string &name)
{
    if (words.fail())
    {
        TraceLog(LOG_WARNING, "Asset '%s' has missing or bad columns in the asset list", name.c_str());
    }
}

// Texture Name file [smooth]
void Assets::addTexture(const std::string &name, std::istringstream &words)
{
    std::string file;
    words >> file;
    CheckColumns(words, name);

    std::string filter; // optional, so it is read after the check
    words >> filter;
    // "smooth" is for large paintings shown scaled down; pixel art keeps its hard edges.
    m_textures[name] = loadTexture(file, filter == "smooth" ? TEXTURE_FILTER_TRILINEAR : TEXTURE_FILTER_POINT);
}

// Box Name file x y width height cornerX cornerY
void Assets::addBox(const std::string &name, std::istringstream &words)
{
    std::string file;
    Rectangle part{};
    float cornerX = 0.0f;
    float cornerY = 0.0f;
    words >> file >> part.x >> part.y >> part.width >> part.height >> cornerX >> cornerY;
    CheckColumns(words, name);
    m_boxes[name] = BoxImage{loadTexture(file, TEXTURE_FILTER_POINT), part, cornerX, cornerY};
}

// Animation Name sheet plainSheet speed
void Assets::addAnimation(const std::string &name, std::istringstream &words)
{
    std::string sheet;
    std::string plainSheet;
    int speed = 1;
    words >> sheet >> plainSheet >> speed;
    CheckColumns(words, name);

    Texture2D texture = loadTexture(sheet, TEXTURE_FILTER_POINT);
    Texture2D silhouette = loadSilhouette(plainSheet);
    int frameCount = std::max(1, texture.width / spriteFrameSize);
    m_animations[name] = Animation(name, texture, silhouette, static_cast<std::size_t>(frameCount), static_cast<std::size_t>(speed));
}

// Font Name file size
void Assets::addFont(const std::string &name, std::istringstream &words)
{
    std::string file;
    int size = 20;
    words >> file >> size;
    CheckColumns(words, name);

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

// Sound Name file
void Assets::addSound(const std::string &name, std::istringstream &words)
{
    std::string file;
    words >> file;
    CheckColumns(words, name);

    std::string fullPath = path(file);
    m_sounds[name] = checkFile(fullPath) ? LoadSound(fullPath.c_str()) : Sound{};
}

// Loads the asset described by one line of the asset list. The first word is the kind of
// asset, the second its name; the add... function of that kind reads the remaining columns.
void Assets::loadLine(const std::string &line)
{
    std::istringstream words(line);
    std::string type;
    bool blankLine = !(words >> type);
    if (blankLine || type[0] == '#')
    {
        return;
    }
    std::string name;
    words >> name;

    if (type == "Texture")
    {
        addTexture(name, words);
    }
    else if (type == "Box")
    {
        addBox(name, words);
    }
    else if (type == "Animation")
    {
        addAnimation(name, words);
    }
    else if (type == "Font")
    {
        addFont(name, words);
    }
    else if (type == "Sound")
    {
        addSound(name, words);
    }
    else
    {
        TraceLog(LOG_WARNING, "Unknown asset type '%s' in the asset list", type.c_str());
    }
}

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
        loadLine(line);
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
