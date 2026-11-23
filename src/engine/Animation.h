#pragma once
#include <cstddef>
#include <string>
#include <raylib.h>

// A sprite sheet split into equal frames laid out left to right.
class Animation
{
    std::string m_name = "none";
    Texture2D m_texture{};
    Texture2D m_silhouette{};
    std::size_t m_frameCount = 1;
    std::size_t m_currentFrame = 0;
    std::size_t m_speed = 1; // game frames per animation frame

public:
    Animation() = default;
    Animation(const std::string &name, Texture2D texture, Texture2D silhouette, std::size_t frameCount, std::size_t speed);

    void update();
    bool hasEnded() const;
    std::size_t frame(bool repeat) const;

    const std::string &getName() const { return m_name; }
    Texture2D getTexture() const { return m_texture; }
    Texture2D getSilhouette() const { return m_silhouette; }
};
