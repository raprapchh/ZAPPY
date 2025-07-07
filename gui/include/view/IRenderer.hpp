#pragma once
#include "../Common.hpp"

namespace zappy {

class World;
class Camera;

class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void render(const World& world, const Camera& camera, float deltaTime) = 0;
    virtual void resize(unsigned int width, unsigned int height) = 0;
    virtual void setQuality(int quality) = 0;
    virtual void setShowGrid(bool show) = 0;
    virtual void setShowDebug(bool show) = 0;
    virtual bool isWindowReady() const = 0;
    virtual bool shouldClose() const = 0;
    virtual void handleEvents() = 0;
    virtual void triggerElevationAnimation(sf::Vector2i position, int level, const sf::Color& teamColor) { (void)position; (void)level; (void)teamColor; }
    virtual void triggerBroadcastAnimation(sf::Vector2i position, float intensity, const sf::Color& teamColor) { (void)position; (void)intensity; (void)teamColor; }
    virtual void triggerDeathAnimation(sf::Vector2i position) { (void)position; }
};

}