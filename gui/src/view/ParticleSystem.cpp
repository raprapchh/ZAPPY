#include "view/ParticleSystem.hpp"
#include "utils/Math.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <cstdlib>

namespace zappy {

void Particle::update(float deltaTime) 
{
    position += velocity * deltaTime;
    rotation += rotationSpeed * deltaTime;
    life -= deltaTime;
    if (type == ParticleType::RESOURCE_PICKUP || type == ParticleType::EGG_HATCH) {
        velocity.y += 50.0f * deltaTime;
    }
    if (type == ParticleType::DEATH || type == ParticleType::BROADCAST) {
        velocity *= 0.98f;
    }
}

ParticleSystem::ParticleSystem()
    : maxParticles_(1000)
    , quality_(1)
    , enabled_(true) 
{
    particles_.reserve(maxParticles_);
}


bool ParticleSystem::initialize() 
{
    particles_.clear();
    LOG_DEBUG("ParticleSystem initialized");
    return true;
}

void ParticleSystem::update(float deltaTime) 
{
    for (auto& particle : particles_) {
        particle.update(deltaTime);
    }
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
                      [](const Particle& p) { return !p.isAlive(); }),
        particles_.end()
    );
}

void ParticleSystem::render(sf::RenderTarget& target, const sf::View& view) 
{
    (void)view;
    sf::CircleShape circle;
    for (const auto& particle : particles_) {
        circle.setRadius(particle.size);
        circle.setOrigin(particle.size, particle.size);
        circle.setPosition(particle.position);
        sf::Color color = particle.color;
        color.a = static_cast<sf::Uint8>(255.0f * particle.getAlpha());
        circle.setFillColor(color);
        circle.setRotation(particle.rotation);
        target.draw(circle);
    }
}

void ParticleSystem::emitElevation(float x, float y, int level) 
{
    int particleCount = 30 + level * 8;
    
    for (int i = 0; i < particleCount; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (i * 360.0f / particleCount) * M_PI / 180.0f;
        float speed = (rand() % 150 + 100) / 100.0f;
        sf::Vector2f velocity(
            cos(angle) * speed,
            sin(angle) * speed - 2.0f
        );
        sf::Color color(255, 215, 0, 255);
        createParticle(ParticleType::ELEVATION, x, y, color, 3.0f + level * 0.3f, 4.0f + level * 0.5f, velocity);
    }
    
    for (int i = 0; i < level * 5; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float spiralAngle = (i * 45.0f) * M_PI / 180.0f;
        float spiralRadius = 20.0f + i * 3.0f;
        sf::Vector2f spiralPos(
            x + cos(spiralAngle) * spiralRadius,
            y + sin(spiralAngle) * spiralRadius
        );
        sf::Vector2f velocity(0, -(rand() % 50 + 30) / 30.0f);
        sf::Color color(255, 255, 150, 200);
        createParticle(ParticleType::ELEVATION, spiralPos.x, spiralPos.y, color, 2.5f, 3.0f, velocity);
    }
    
    for (int i = 0; i < level * 3; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float starX = x + (rand() % 80 - 40);
        float starY = y + (rand() % 80 - 40);
        sf::Color starColor(255, 255, 255, 180);
        createParticle(ParticleType::ELEVATION, starX, starY, starColor, 1.5f + level * 0.2f, 2.0f, sf::Vector2f(0, 0));
    }
}

void ParticleSystem::emitDeath(float x, float y, const sf::Color& teamColor) 
{
    int particleCount = 30;
    for (int i = 0; i < particleCount; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (i * 360.0f / particleCount) * M_PI / 180.0f;
        float speed = (rand() % 100 + 50) / 50.0f;
        sf::Vector2f velocity(
            cos(angle) * speed,
            sin(angle) * speed
        );
        sf::Color color = teamColor;
        color.a = 200;
        createParticle(ParticleType::DEATH, x, y, color, 1.5f, 4.0f, velocity);
    }
}

void ParticleSystem::emitResourcePickup(float x, float y, ResourceType type) 
{
    int particleCount = 10;
    sf::Color color;
    switch (type) {
        case ResourceType::Food: color = sf::Color(139, 69, 19); break;
        case ResourceType::Linemate: color = sf::Color(128, 128, 128); break;
        case ResourceType::Deraumere: color = sf::Color(255, 0, 0); break;
        case ResourceType::Sibur: color = sf::Color(0, 255, 0); break;
        case ResourceType::Mendiane: color = sf::Color(0, 0, 255); break;
        case ResourceType::Phiras: color = sf::Color(255, 255, 0); break;
        case ResourceType::Thystame: color = sf::Color(255, 0, 255); break;
        case ResourceType::Count: color = sf::Color::White; break;
    }
    for (int i = 0; i < particleCount; ++i) {
        if (particles_.size() >= maxParticles_) break;
        sf::Vector2f velocity(
            (rand() % 60 - 30) / 30.0f,
            -(rand() % 50 + 25) / 25.0f
        );
        createParticle(ParticleType::RESOURCE_PICKUP, x, y, color, 1.0f, 2.0f, velocity);
    }
}

void ParticleSystem::emitBroadcast(float x, float y, float intensity) 
{
    int waveParticles = static_cast<int>(25 * intensity);
    
    for (int wave = 0; wave < 3; ++wave) {
        for (int i = 0; i < waveParticles; ++i) {
            if (particles_.size() >= maxParticles_) break;
            float angle = (i * 360.0f / waveParticles) * M_PI / 180.0f;
            float waveRadius = 20.0f + wave * 30.0f;
            float speed = intensity * 1.5f + wave * 0.5f;
            
            sf::Vector2f startPos(
                x + cos(angle) * (waveRadius * 0.3f),
                y + sin(angle) * (waveRadius * 0.3f)
            );
            sf::Vector2f velocity(
                cos(angle) * speed,
                sin(angle) * speed
            );
            
            sf::Color waveColor;
            switch (wave) {
                case 0: waveColor = sf::Color(100, 200, 255, 180); break;
                case 1: waveColor = sf::Color(150, 220, 255, 150); break;
                case 2: waveColor = sf::Color(200, 240, 255, 120); break;
            }
            
            createParticle(ParticleType::BROADCAST, startPos.x, startPos.y, waveColor, 
                         2.5f + wave * 0.5f, 3.0f + wave * 0.5f, velocity);
        }
    }
    
    int centerParticles = static_cast<int>(10 * intensity);
    for (int i = 0; i < centerParticles; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (i * 360.0f / centerParticles) * M_PI / 180.0f;
        float radius = rand() % 15;
        sf::Vector2f centerPos(
            x + cos(angle) * radius,
            y + sin(angle) * radius
        );
        sf::Vector2f velocity(
            (rand() % 40 - 20) / 20.0f,
            (rand() % 40 - 20) / 20.0f
        );
        sf::Color centerColor(255, 255, 255, 200);
        createParticle(ParticleType::BROADCAST, centerPos.x, centerPos.y, centerColor, 1.5f, 2.0f, velocity);
    }
    
    int trailParticles = static_cast<int>(15 * intensity);
    for (int i = 0; i < trailParticles; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (rand() % 360) * M_PI / 180.0f;
        float distance = 40.0f + rand() % 60;
        sf::Vector2f trailPos(
            x + cos(angle) * distance,
            y + sin(angle) * distance
        );
        sf::Vector2f velocity(
            cos(angle + M_PI) * 0.5f,
            sin(angle + M_PI) * 0.5f
        );
        sf::Color trailColor(180, 220, 255, 100);
        createParticle(ParticleType::BROADCAST, trailPos.x, trailPos.y, trailColor, 3.0f, 1.5f, velocity);
    }
}

void ParticleSystem::emitIncantation(float x, float y, int level, float progress) 
{
    int particleCount = 5 + level * 2;
    for (int i = 0; i < particleCount; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (progress * 360.0f + i * 45.0f) * M_PI / 180.0f;
        float radius = 20.0f + level * 5.0f;
        sf::Vector2f offset(
            cos(angle) * radius,
            sin(angle) * radius
        );
        sf::Vector2f velocity(
            -sin(angle) * 0.5f,
            cos(angle) * 0.5f
        );
        sf::Color color(255, 100 + level * 20, 255, 180);
        createParticle(ParticleType::INCANTATION, x, y, color, 1.0f, 2.0f + level, velocity);
    }
}

void ParticleSystem::emitEggHatch(float x, float y, const sf::Color& teamColor) 
{
    int particleCount = 15;
    for (int i = 0; i < particleCount; ++i) {
        if (particles_.size() >= maxParticles_) break;
        float angle = (i * 360.0f / particleCount) * M_PI / 180.0f;
        float speed = (rand() % 80 + 40) / 40.0f;
        sf::Vector2f velocity(
            cos(angle) * speed,
            sin(angle) * speed - 1.0f
        );
        sf::Color color = teamColor;
        color.a = 180;
        createParticle(ParticleType::EGG_HATCH, x, y, color, 1.5f, 3.0f, velocity);
    }
}

void ParticleSystem::setQuality(int quality) 
{
    quality_ = std::clamp(quality, 0, 3);
}

void ParticleSystem::setMaxParticles(size_t maxParticles) 
{
    maxParticles_ = maxParticles;
    particles_.reserve(maxParticles_);
}

size_t ParticleSystem::getParticleCount() const 
{
    return particles_.size();
}

size_t ParticleSystem::getActiveParticles() const 
{
    size_t activeCount = 0;
    for (const auto& p : particles_) {
        if (p.isAlive()) {
            activeCount++;
        }
    }
    return activeCount;
}

void ParticleSystem::createParticle(ParticleType type, float x, float y, const sf::Color& color, float size, float lifetime, sf::Vector2f velocity) 
{
    if (particles_.size() >= maxParticles_) return;
    Particle particle;
    particle.position = sf::Vector2f(x, y);
    particle.velocity = velocity;
    particle.color = color;
    particle.life = lifetime;
    particle.maxLife = lifetime;
    particle.size = size;
    particle.rotation = 0.0f;
    particle.rotationSpeed = (rand() % 360 - 180) / 180.0f * M_PI;
    particle.type = type;
    particles_.push_back(particle);
}

void ParticleSystem::removeDeadParticles() 
{
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
                      [](const Particle& p) { return !p.isAlive(); }),
        particles_.end()
    );
}

void ParticleSystem::cullInvisibleParticles(const sf::View& view) 
{
    (void)view;
}

}
