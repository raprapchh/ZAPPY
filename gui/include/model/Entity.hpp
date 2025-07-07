#pragma once
#include "Common.hpp"
namespace zappy {
class Entity {
public:
    explicit Entity(int id);
    virtual ~Entity() = default;

    virtual sf::Vector2f getPosition() const = 0;
    virtual void setPosition(const sf::Vector2f& position) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void update(float deltaTime) = 0;

    int getId() const;
    bool isVisible() const;
    void setVisible(bool visible);

    virtual bool isPlayer() const;
    virtual bool isEgg() const;
    virtual bool isResource() const;

    void setTargetPosition(const sf::Vector2f& target);
    void clearTargetPosition();

protected:
    void updateInterpolation(float deltaTime);

    int id_;
    bool visible_{true};
    sf::Vector2f currentPosition_{0.0f, 0.0f};
    sf::Vector2f targetPosition_{0.0f, 0.0f};
    bool hasTargetPosition_{false};
    float interpolationSpeed_{5.0f};
};

template<typename Derived>
class EntityBase : public Entity {
public:
    explicit EntityBase(int id) : Entity(id) {}
    static constexpr bool is_player = std::is_same_v<Derived, class Player>;
    static constexpr bool is_egg = std::is_same_v<Derived, class Egg>;
    bool isPlayer() const override { return is_player; }
    bool isEgg() const override { return is_egg; }
};
}
