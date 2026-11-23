#include "engine/Animation.h"
#include <algorithm>

Animation::Animation(const std::string &name, Texture2D texture, Texture2D silhouette, std::size_t frameCount, std::size_t speed)
    : m_name(name), m_texture(texture), m_silhouette(silhouette),
      m_frameCount(std::max<std::size_t>(1, frameCount)), m_speed(std::max<std::size_t>(1, speed))
{
}

void Animation::update()
{
    m_currentFrame++;
}

bool Animation::hasEnded() const
{
    return m_currentFrame / m_speed >= m_frameCount;
}

std::size_t Animation::frame(bool repeat) const
{
    std::size_t frame = m_currentFrame / m_speed;
    return repeat ? frame % m_frameCount : std::min(frame, m_frameCount - 1);
}
