#include "view/Camera.hpp"
#include "utils/Math.hpp"
#include "utils/Logger.hpp"
#include <algorithm>

namespace zappy {

Camera::Camera()
    : mode_(CameraMode::FREE)
    , position_(0.0f, 0.0f)
    , targetPosition_(0.0f, 0.0f)
    , zoom_(1.0f)
    , targetZoom_(1.0f)
    , viewSize_(800.0f, 600.0f)
    , minBounds_(0.0f, 0.0f)
    , maxBounds_(1000.0f, 1000.0f)
    , followingPlayerId_(0)
    , followingTeamName_("")
    , isFollowing_(false)
    , keys_{}
    , mousePressed_(false)
    , lastMousePos_(0, 0)
    , dragStart_(0.0f, 0.0f)
    , moveSpeed_(300.0f)
    , zoomSpeed_(2.0f)
    , smoothness_(5.0f)
    , minZoom_(0.1f)
    , maxZoom_(5.0f)
    , lastPosition_(0.0f, 0.0f)
{
}

void Camera::update(float deltaTime) 
{
    updateFollowTarget();
    smoothMovement(deltaTime);
    constrainPosition();
    lastPosition_ = position_;
}

void Camera::handleInput(const sf::Event& event) 
{
    switch (event.type) {
        case sf::Event::KeyPressed:
            onKeyPressed(event.key.code);
            break;
        case sf::Event::KeyReleased:
            onKeyReleased(event.key.code);
            break;
        case sf::Event::MouseButtonPressed:
            onMouseButtonPressed(event.mouseButton.button,
                               event.mouseButton.x, event.mouseButton.y);
            break;
        case sf::Event::MouseButtonReleased:
            onMouseButtonReleased(event.mouseButton.button,
                                event.mouseButton.x, event.mouseButton.y);
            break;
        case sf::Event::MouseMoved:
            onMouseMoved(event.mouseMove.x, event.mouseMove.y);
            break;
        case sf::Event::MouseWheelScrolled:
            onMouseWheelScrolled(event.mouseWheelScroll.delta);
            break;
        default:
            break;
    }
}

void Camera::setMode(CameraMode mode) 
{
    mode_ = mode;
    resetView();
}

CameraMode Camera::getMode() const 
{
    return mode_;
}

void Camera::followPlayer(size_t playerId) 
{
    mode_ = CameraMode::FOLLOW;
    followingPlayerId_ = playerId;
    followingTeamName_.clear();
    isFollowing_ = true;
}

void Camera::followTeam(const std::string& teamName) 
{
    mode_ = CameraMode::TEAM;
    followingTeamName_ = teamName;
    followingPlayerId_ = 0;
    isFollowing_ = true;
}

void Camera::stopFollowing() 
{
    mode_ = CameraMode::FREE;
    followingPlayerId_ = 0;
    followingTeamName_.clear();
    isFollowing_ = false;
}

void Camera::setPosition(float x, float y) 
{
    position_ = sf::Vector2f(x, y);
    targetPosition_ = position_;
}

void Camera::move(float dx, float dy) 
{
    targetPosition_.x += dx;
    targetPosition_.y += dy;
}

sf::Vector2f Camera::getPosition() const 
{
    return position_;
}

void Camera::setZoom(float zoom) 
{
    zoom_ = std::clamp(zoom, minZoom_, maxZoom_);
    targetZoom_ = zoom_;
}

void Camera::zoom(float factor) 
{
    setZoom(zoom_ * factor);
}

float Camera::getZoom() const 
{
    return zoom_;
}

sf::FloatRect Camera::getViewBounds() const 
{
    sf::Vector2f size = viewSize_ * zoom_;
    return sf::FloatRect(position_.x - size.x / 2, position_.y - size.y / 2, size.x, size.y);
}

sf::View Camera::getView() const 
{
    sf::View view;
    view.setCenter(position_);
    view.setSize(viewSize_ * zoom_);
    return view;
}

void Camera::setViewSize(float width, float height) 
{
    viewSize_ = sf::Vector2f(width, height);
}

void Camera::setBounds(float minX, float minY, float maxX, float maxY) 
{
    minBounds_ = sf::Vector2f(minX, minY);
    maxBounds_ = sf::Vector2f(maxX, maxY);
}

void Camera::onKeyPressed(sf::Keyboard::Key key) 
{
    if (key >= 0 && key < sf::Keyboard::KeyCount) {
        keys_[key] = true;
    }
}

void Camera::onKeyReleased(sf::Keyboard::Key key) 
{
    if (key >= 0 && key < sf::Keyboard::KeyCount) {
        keys_[key] = false;
    }
}

void Camera::onMouseButtonPressed(sf::Mouse::Button button, int x, int y)
{
    if (button == sf::Mouse::Middle ||
        (button == sf::Mouse::Left && mode_ == CameraMode::FREE)) {
        mousePressed_ = true;
        lastMousePos_ = sf::Vector2i(x, y);
        dragStart_ = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
    }
}

void Camera::onMouseButtonReleased(sf::Mouse::Button button, int , int ) 
{
    if (button == sf::Mouse::Middle || button == sf::Mouse::Left) {
        mousePressed_ = false;
    }
}

void Camera::onMouseMoved(int x, int y) 
{
    if (mousePressed_ && (mode_ == CameraMode::FREE || mode_ == CameraMode::FOLLOW)) {
        sf::Vector2i currentPos(x, y);
        sf::Vector2i delta = lastMousePos_ - currentPos;
        sf::Vector2f worldDelta(
            static_cast<float>(delta.x) * zoom_,
            static_cast<float>(delta.y) * zoom_
        );
        move(worldDelta.x, worldDelta.y);
        lastMousePos_ = currentPos;
    }
}

void Camera::onMouseWheelScrolled(float delta) 
{
    float zoomFactor = 1.0f + delta * 0.1f;
    zoom(zoomFactor);
}

sf::Vector2f Camera::screenToWorld(int x, int y) const 
{
    sf::Vector2f screenPos(static_cast<float>(x), static_cast<float>(y));
    sf::Vector2f viewCenter = position_;
    sf::Vector2f worldPos;
    worldPos.x = viewCenter.x + (screenPos.x - viewSize_.x / 2) * zoom_;
    worldPos.y = viewCenter.y + (screenPos.y - viewSize_.y / 2) * zoom_;
    return worldPos;
}

sf::Vector2i Camera::worldToScreen(float x, float y) const 
{
    sf::Vector2f worldPos(x, y);
    sf::Vector2f viewCenter = position_;
    sf::Vector2i screenPos;
    screenPos.x = static_cast<int>((worldPos.x - viewCenter.x) / zoom_ + viewSize_.x / 2);
    screenPos.y = static_cast<int>((worldPos.y - viewCenter.y) / zoom_ + viewSize_.y / 2);
    return screenPos;
}

bool Camera::hasMoved(float threshold) const 
{
    sf::Vector2f delta = position_ - lastPosition_;
    return math::length(delta) > threshold;
}

void Camera::resetView() 
{
    switch (mode_) {
        case CameraMode::FREE:
            break;
        case CameraMode::FOLLOW:
            break;
        case CameraMode::TEAM:
            break;
        case CameraMode::OVERVIEW:
            float mapWidth = maxBounds_.x - minBounds_.x;
            float mapHeight = maxBounds_.y - minBounds_.y;
            if (mapWidth > 0 && mapHeight > 0) {
                float zoomX = viewSize_.x / mapWidth;
                float zoomY = viewSize_.y / mapHeight;
                float optimalZoom = std::min(zoomX, zoomY) * 1.0f;
                setZoom(std::max(0.1f, std::min(2.0f, optimalZoom)));
                setPosition((minBounds_.x + maxBounds_.x) / 2, (minBounds_.y + maxBounds_.y) / 2);
            } else {
                setZoom(0.5f);
                setPosition(320.0f, 320.0f);
            }
            break;
    }
}

void Camera::updateFollowTarget() 
{
    if (!isFollowing_) return;
}

void Camera::constrainPosition() 
{
    sf::Vector2f halfView = viewSize_ * zoom_ * 0.5f;
    float minX = minBounds_.x + halfView.x;
    float maxX = maxBounds_.x - halfView.x;
    float minY = minBounds_.y + halfView.y;
    float maxY = maxBounds_.y - halfView.y;
    if (maxX < minX) {
        targetPosition_.x = (minBounds_.x + maxBounds_.x) / 2.0f;
    } else {
        targetPosition_.x = std::clamp(targetPosition_.x, minX, maxX);
    }
    if (maxY < minY) {
        targetPosition_.y = (minBounds_.y + maxBounds_.y) / 2.0f;
    } else {
        targetPosition_.y = std::clamp(targetPosition_.y, minY, maxY);
    }
}

void Camera::smoothMovement(float deltaTime) 
{
    if (mode_ == CameraMode::FREE) {
        sf::Vector2f movement(0, 0);
        float speed = moveSpeed_ * deltaTime * zoom_;
        if (keys_[sf::Keyboard::W] || keys_[sf::Keyboard::Up]) {
            movement.y -= speed;
        }
        if (keys_[sf::Keyboard::S] || keys_[sf::Keyboard::Down]) {
            movement.y += speed;
        }
        if (keys_[sf::Keyboard::A] || keys_[sf::Keyboard::Left]) {
            movement.x -= speed;
        }
        if (keys_[sf::Keyboard::D] || keys_[sf::Keyboard::Right]) {
            movement.x += speed;
        }
        if (movement.x != 0 || movement.y != 0) {
            move(movement.x, movement.y);
        }
    }
    if (math::length(targetPosition_ - position_) > 0.1f) {
        sf::Vector2f delta = targetPosition_ - position_;
        position_ += delta * smoothness_ * deltaTime;
    } else {
        position_ = targetPosition_;
    }
    if (std::abs(targetZoom_ - zoom_) > 0.01f) {
        float delta = targetZoom_ - zoom_;
        zoom_ += delta * smoothness_ * deltaTime;
    } else {
        zoom_ = targetZoom_;
    }
}

}
