#pragma once
#include "Common.hpp"
namespace zappy {
enum class ParticleType {
    ELEVATION,
    DEATH,
    RESOURCE_PICKUP,
    BROADCAST,
    INCANTATION,
    EGG_HATCH
};
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float life;
    float maxLife;
    float size;
    float rotation;
    float rotationSpeed;
    ParticleType type;
    void update(float deltaTime);
    bool isAlive() const { return life > 0.0f; }
    float getAlpha() const { return life / maxLife; }
};
class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem() = default;

    bool initialize();
    void update(float deltaTime);
    void render(sf::RenderTarget& target, const sf::View& view);

    void emitElevation(float x, float y, int level);
    void emitDeath(float x, float y, const sf::Color& teamColor);
    void emitResourcePickup(float x, float y, ResourceType type);
    void emitBroadcast(float x, float y, float intensity);
    void emitIncantation(float x, float y, int level, float progress);
    void emitEggHatch(float x, float y, const sf::Color& teamColor);

    void setQuality(int quality);
    void setMaxParticles(size_t maxParticles);
    size_t getParticleCount() const;
    size_t getActiveParticles() const;

private:
    void createParticle(ParticleType type, float x, float y, const sf::Color& color,
                       float life, float size, sf::Vector2f velocity = {0, 0});
    void removeDeadParticles();
    void cullInvisibleParticles(const sf::View& view);

    std::vector<Particle> particles_;
    size_t maxParticles_;
    sf::CircleShape particleShape_;
    sf::RectangleShape particleRect_;
    int quality_;
    bool enabled_;
};
}
