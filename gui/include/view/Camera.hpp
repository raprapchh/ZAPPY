#pragma once
#include "Common.hpp"
namespace zappy {
enum class CameraMode {
    FREE,
    FOLLOW,
    TEAM,
    OVERVIEW
};
class Camera {
public:
    Camera();
    ~Camera() = default;

    void update(float deltaTime);
    void handleInput(const sf::Event& event);

    void setMode(CameraMode mode);
    CameraMode getMode() const;

    void followPlayer(size_t playerId);
    void followTeam(const std::string& teamName);
    void stopFollowing();

    void setPosition(float x, float y);
    void move(float dx, float dy);
    sf::Vector2f getPosition() const;

    void setZoom(float zoom);
    void zoom(float factor);
    float getZoom() const;

    sf::FloatRect getViewBounds() const;
    sf::View getView() const;

    void setViewSize(float width, float height);
    void setBounds(float minX, float minY, float maxX, float maxY);

    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    void onMouseButtonPressed(sf::Mouse::Button button, int x, int y);
    void onMouseButtonReleased(sf::Mouse::Button button, int x, int y);
    void onMouseMoved(int x, int y);
    void onMouseWheelScrolled(float delta);

    sf::Vector2f screenToWorld(int x, int y) const;
    sf::Vector2i worldToScreen(float x, float y) const;

    bool hasMoved(float threshold = 0.1f) const;
    void resetView();

private:
    void updateFollowTarget();
    void constrainPosition();
    void smoothMovement(float deltaTime);

    CameraMode mode_;
    sf::Vector2f position_;
    sf::Vector2f targetPosition_;
    float zoom_;
    float targetZoom_;
    sf::Vector2f viewSize_;
    sf::Vector2f minBounds_;
    sf::Vector2f maxBounds_;
    size_t followingPlayerId_;
    std::string followingTeamName_;
    bool isFollowing_;
    bool keys_[sf::Keyboard::KeyCount];
    bool mousePressed_;
    sf::Vector2i lastMousePos_;
    sf::Vector2f dragStart_;
    float moveSpeed_;
    float zoomSpeed_;
    float smoothness_;
    float minZoom_;
    float maxZoom_;
    sf::Vector2f lastPosition_;
};
}
