#include "ui/HUD.hpp"
#include "utils/ResourceManager.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace zappy {

HUD::HUD(bool showFPS)
    : windowSize_(800, 600)
    , barHeight_(40.0f)
    , gameTime_(0.0f)
    , timeMultiplier_(1.0f)
    , currentFPS_(0.0f)
    , fpsUpdateTimer_(0.0f)
    , showFPS_(showFPS)
{
}

bool HUD::initialize(const sf::Vector2u& windowSize) 
{
    windowSize_ = windowSize;
    if (!ResourceManager::getInstance().loadFont("default", "")) {
        LOG_WARNING("Failed to load HUD font, using default");
    }
    font_ = ResourceManager::getInstance().getFont("default");
    setupLayout();
    timeText_.setFont(font_);
    timeText_.setCharacterSize(16);
    timeText_.setFillColor(sf::Color::White);
    fpsText_.setFont(font_);
    fpsText_.setCharacterSize(14);
    fpsText_.setFillColor(sf::Color::Yellow);
    playerCountText_.setFont(font_);
    playerCountText_.setCharacterSize(14);
    playerCountText_.setFillColor(sf::Color::Cyan);
    LOG_INFO("HUD initialized successfully");
    return true;
}

void HUD::update(float deltaTime, const World& ) 
{
    fpsUpdateTimer_ += deltaTime;
    if (fpsUpdateTimer_ >= 0.5f) {
        fpsUpdateTimer_ = 0.0f;
    }
    gameTime_ += deltaTime * timeMultiplier_;
}

void HUD::render(sf::RenderTarget& target) 
{
    renderTimeInfo(target);
}

void HUD::resize(const sf::Vector2u& windowSize) 
{
    windowSize_ = windowSize;
    setupLayout();
}

void HUD::setShowFPS(bool show) 
{
    showFPS_ = show;
}

void HUD::setTimeMultiplier(float multiplier) 
{
    timeMultiplier_ = multiplier;
}

void HUD::updateFPS(float fps) 
{
    currentFPS_ = fps;
}

void HUD::setGameTime(float time) 
{
    gameTime_ = time;
}

void HUD::setupLayout() 
{
    float scaleFactor = static_cast<float>(windowSize_.y) / 600.0f;
    barHeight_ = 40.0f * scaleFactor;
    topBarArea_ = sf::FloatRect(0, 0, static_cast<float>(windowSize_.x), barHeight_);
    timeText_.setCharacterSize(static_cast<unsigned int>(16 * scaleFactor));
    timeText_.setPosition(10 * scaleFactor, 10 * scaleFactor);
    fpsText_.setCharacterSize(static_cast<unsigned int>(14 * scaleFactor));
    fpsText_.setPosition(static_cast<float>(windowSize_.x) - (80 * scaleFactor), 10 * scaleFactor);
    playerCountText_.setCharacterSize(static_cast<unsigned int>(14 * scaleFactor));
}

void HUD::renderResourceBar(sf::RenderTarget& target, const World& ) 
{
    (void)target;
}

void HUD::renderTimeInfo(sf::RenderTarget& target) 
{
    int minutes = static_cast<int>(gameTime_) / 60;
    int seconds = static_cast<int>(gameTime_) % 60;
    std::ostringstream timeStream;
    timeStream << "Time: " << std::setfill('0') << std::setw(2) << minutes
               << ":" << std::setfill('0') << std::setw(2) << seconds;
    timeText_.setString(timeStream.str());
    timeText_.setPosition(10, 10);
    target.draw(timeText_);
}

void HUD::renderFPSCounter(sf::RenderTarget& target) 
{
    std::ostringstream fpsStream;
    fpsStream << "FPS: " << std::fixed << std::setprecision(1) << currentFPS_;
    fpsText_.setString(fpsStream.str());
    fpsText_.setPosition(static_cast<float>(windowSize_.x) - 80, 10);
    target.draw(fpsText_);
}

}
