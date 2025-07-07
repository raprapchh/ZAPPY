#pragma once
#include "Common.hpp"
#include "model/World.hpp"
namespace zappy {
class HUD {
public:
    explicit HUD(bool showFPS = false);
    ~HUD() = default;

    bool initialize(const sf::Vector2u& windowSize);
    void update(float deltaTime, const World& world);
    void render(sf::RenderTarget& target);
    void resize(const sf::Vector2u& windowSize);

    void setShowFPS(bool show);
    void setTimeMultiplier(float multiplier);
    void updateFPS(float fps);
    void setGameTime(float time);

private:
    void setupLayout();
    void renderResourceBar(sf::RenderTarget& target, const World& world);
    void renderTimeInfo(sf::RenderTarget& target);
    void renderFPSCounter(sf::RenderTarget& target);

    sf::Vector2u windowSize_;
    sf::FloatRect topBarArea_;
    float barHeight_;
    sf::Font font_;
    sf::Text timeText_;
    sf::Text speedText_;
    sf::Text fpsText_;
    sf::Text playerCountText_;
    std::vector<sf::Text> resourceTexts_;
    sf::RectangleShape background_;
    sf::RectangleShape separator_;
    float gameTime_;
    float timeMultiplier_;
    float currentFPS_;
    float fpsUpdateTimer_;
    bool showFPS_;
    sf::Color backgroundColor_;
    sf::Color textColor_;
    sf::Color separatorColor_;
};
}
