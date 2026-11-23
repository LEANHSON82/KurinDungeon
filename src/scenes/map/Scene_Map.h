#pragma once
#include <memory>
#include <string>
#include <vector>
#include "engine/Scene.h"
#include "scenes/map/MapGenerator.h"

class Scene_Map : public Scene
{
    std::shared_ptr<Entity> m_start;
    std::shared_ptr<Entity> m_currentRoom;
    EntityVec m_rooms;

    void layoutRooms();

    // Drawing
    std::shared_ptr<Entity> roomAt(Vector2 point) const;
    std::shared_ptr<Entity> visitableRoomAt(Vector2 point) const;
    std::string describeRoom(const std::shared_ptr<Entity> &room) const;
    const char *describeState(const std::shared_ptr<Entity> &room) const;
    void drawPaths() const;
    void drawRooms(const std::shared_ptr<Entity> &hovered) const;
    void drawRoomInfo(const std::shared_ptr<Entity> &room) const;
    void drawLegend() const;

public:
    explicit Scene_Map(GameEngine *game);

    void update() override;
    void sDoAction(const Action &action) override;
    void sRender() override;

    // Public so the tests can walk through a map.
    bool canVisit(const std::shared_ptr<Entity> &from, const std::shared_ptr<Entity> &to) const;
    void enterRoom(const std::shared_ptr<Entity> &room);
    void finishCurrentRoom();
    std::shared_ptr<Entity> currentRoom() const { return m_currentRoom; }
    const EntityVec &rooms() const { return m_rooms; }
};
