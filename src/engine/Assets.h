#pragma once
#include <map>
#include <string>
#include <raylib.h>
#include "engine/Animation.h"

// A texture drawn as a nine-slice box: corners stay the same size, edges and middle stretch.
struct BoxImage
{
    Texture2D texture{};
    Rectangle part{};
    float cornerX = 0.0f;
    float cornerY = 0.0f;
};

// Loads every texture, animation, font and sound listed in assets.txt and hands them out by name.
class Assets
{
    std::string m_folder;
    std::map<std::string, Texture2D> m_textures;
    std::map<std::string, BoxImage> m_boxes;
    std::map<std::string, Animation> m_animations;
    std::map<std::string, Font> m_fonts;
    std::map<std::string, Sound> m_sounds;
    int m_missingFiles = 0;

    std::string path(const std::string &file) const;
    bool checkFile(const std::string &fullPath);
    Texture2D loadTexture(const std::string &file, int filter);
    Texture2D loadSilhouette(const std::string &file);

    void addTexture(const std::string &name, const std::string &file, int filter);
    void addBox(const std::string &name, const std::string &file, Rectangle part, float cornerX, float cornerY);
    void addAnimation(const std::string &name, const std::string &sheet, const std::string &plainSheet, int speed);
    void addFont(const std::string &name, const std::string &file, int size);
    void addSound(const std::string &name, const std::string &file);

public:
    // folder holds assets.txt; paths inside it are relative to that folder.
    void loadFromFile(const std::string &folder, const std::string &listFile);
    void unload();

    const Texture2D &getTexture(const std::string &name) const;
    const BoxImage &getBox(const std::string &name) const;
    const Animation &getAnimation(const std::string &name) const;
    bool hasAnimation(const std::string &name) const;
    const Font &getFont(const std::string &name) const;
    const Sound &getSound(const std::string &name) const;

    void playSound(const std::string &name) const;
};
