#include "model/Entity.hpp"
#include "utils/Math.hpp"

namespace zappy {

Entity::Entity(int id) : id_(id)
{
}

void Entity::update(float deltaTime)
{
    updateInterpolation(deltaTime);
}

int Entity::getId() const
{
    return id_;
}

bool Entity::isVisible() const
{
    return visible_;
}

void Entity::setVisible(bool visible)
{
    visible_ = visible;
}

bool Entity::isPlayer() const
{
    return false;
}

bool Entity::isEgg() const
{
    return false;
}

bool Entity::isResource() const
{
    return false;
}

void Entity::setTargetPosition(const sf::Vector2f& target)
{
    targetPosition_ = target;
    hasTargetPosition_ = true;
}

void Entity::clearTargetPosition()
{
    hasTargetPosition_ = false;
}

void Entity::updateInterpolation(float deltaTime)
{
    if (!hasTargetPosition_) {
        return;
    }

    sf::Vector2f currentPos = getPosition();
    sf::Vector2f direction = targetPosition_ - currentPos;
    float distance = math::length(direction);

    if (distance < 1.0f) {
        setPosition(targetPosition_);
        clearTargetPosition();
        return;
    }

    float speed = 100.0f;
    float moveDistance = speed * deltaTime;

    if (moveDistance >= distance) {
        setPosition(targetPosition_);
        clearTargetPosition();
    } else {
        sf::Vector2f normalizedDirection = direction / distance;
        sf::Vector2f newPos = currentPos + normalizedDirection * moveDistance;
        setPosition(newPos);
    }
}

}
