#pragma once
#include "Common.hpp"
#include <vector>
#include <memory>

namespace zappy {

enum class TileAnimationType {
    ELEVATION,
    BROADCAST_WAVE,
    DEATH
};

struct TileAnimation {
    sf::Vector2i position;
    TileAnimationType type;
    float duration;
    float elapsed;
    float intensity;
    int playerId;
    int radius;
    sf::Color color;
    
    TileAnimation(sf::Vector2i pos, TileAnimationType type, float duration, float intensity = 1.0f, int playerId = -1)
        : position(pos), type(type), duration(duration), elapsed(0.0f), 
          intensity(intensity), playerId(playerId), radius(0), color(sf::Color::White) {}
    
    bool isFinished() const { return elapsed >= duration; }
    float getProgress() const { return elapsed / duration; }
};

class TileAnimationSystem {
public:
    TileAnimationSystem();
    ~TileAnimationSystem() = default;
    
    void update(float deltaTime);
    void render(sf::RenderTarget& target, const sf::View& view);
    
    void startElevationAnimation(sf::Vector2i position, int level, const sf::Color& teamColor);
    void startBroadcastWave(sf::Vector2i position, float intensity, const sf::Color& teamColor);
    void startDeathAnimation(sf::Vector2i position);
    
    void clear();
    size_t getActiveAnimationCount() const;

private:
    void renderElevationAnimation(sf::RenderTarget& target, const TileAnimation& anim);
    void renderBroadcastWave(sf::RenderTarget& target, const TileAnimation& anim);
    void renderDeathAnimation(sf::RenderTarget& target, const TileAnimation& anim);
    
    std::vector<std::unique_ptr<TileAnimation>> animations_;
    sf::CircleShape circleShape_;
    sf::RectangleShape rectShape_;
};

}
