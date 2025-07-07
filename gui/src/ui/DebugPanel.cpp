#include "ui/DebugPanel.hpp"
#include "utils/ResourceManager.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace zappy {
DebugPanel::DebugPanel()
    : visible_(false)
    , showAllMetrics_(false)
    , fpsHistoryIndex_(0)
    , backgroundColor_(0, 0, 0, 180)
    , borderColor_(100, 100, 100, 255)
    , textColor_(255, 255, 255, 255)
    , graphColor_(0, 255, 0, 255)
    , separatorColor_(150, 150, 150, 255)
    , padding_(10.0f)
    , lineHeight_(16.0f)
    , sectionSpacing_(20.0f)
    , maxFPSHistory_(120) 
{
    fpsHistory_.resize(maxFPSHistory_, 0.0f);
    metricTexts_.resize(20);
    fpsGraph_.setPrimitiveType(sf::LineStrip);
    fpsGraph_.resize(maxFPSHistory_);
}

bool DebugPanel::initialize(const sf::FloatRect& area) 
{
    area_ = area;
    font_ = ResourceManager::getInstance().getFont("default");
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    background_.setFillColor(backgroundColor_);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineThickness(1.0f);
    border_.setOutlineColor(borderColor_);
    float scaleFactor = area.height / 600.0f;
    padding_ = 10.0f * scaleFactor;
    lineHeight_ = 16.0f * scaleFactor;
    sectionSpacing_ = 20.0f * scaleFactor;
    titleText_.setFont(font_);
    titleText_.setString("Debug Panel");
    titleText_.setCharacterSize(static_cast<unsigned int>(14 * scaleFactor));
    titleText_.setFillColor(textColor_);
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
    for (auto& text : metricTexts_) {
        text.setFont(font_);
        text.setCharacterSize(static_cast<unsigned int>(12 * scaleFactor));
        text.setFillColor(textColor_);
    }
    float graphWidth = 200.0f;
    float graphHeight = 60.0f;
    graphBackground_.setSize(sf::Vector2f(graphWidth, graphHeight));
    graphBackground_.setPosition(area_.left + area_.width - graphWidth - padding_,
                                area_.top + padding_ + 30.0f);
    graphBackground_.setFillColor(sf::Color(20, 20, 20, 200));
    graphBackground_.setOutlineThickness(1.0f);
    graphBackground_.setOutlineColor(borderColor_);
    LOG_DEBUG("DebugPanel initialized");
    return true;
}

void DebugPanel::update(float deltaTime, const World& world, const Camera& camera,
        const NetworkClient& network) 
{
    (void)deltaTime;
    (void)world;
    (void)camera;
    (void)network;
    if (!visible_) return;
    calculateMetrics(world, camera, network);
    fpsHistory_[fpsHistoryIndex_] = metrics_.currentFPS;
    fpsHistoryIndex_ = (fpsHistoryIndex_ + 1) % maxFPSHistory_;
    float graphWidth = graphBackground_.getSize().x;
    float graphHeight = graphBackground_.getSize().y;
    sf::Vector2f graphPos = graphBackground_.getPosition();
    for (size_t i = 0; i < maxFPSHistory_; ++i) {
        size_t index = (fpsHistoryIndex_ + i) % maxFPSHistory_;
        float fps = fpsHistory_[index];
        float x = graphPos.x + (static_cast<float>(i) / static_cast<float>(maxFPSHistory_ - 1)) * graphWidth;
        float y = graphPos.y + graphHeight - (fps / 120.0f) * graphHeight;
        fpsGraph_[i].position = sf::Vector2f(x, y);
        fpsGraph_[i].color = graphColor_;
    }
}

void DebugPanel::render(sf::RenderTarget& target) 
{
    if (!visible_) return;
    renderBackground(target);
    renderPerformanceSection(target);
    renderEntitySection(target);
    renderNetworkSection(target);
    if (showAllMetrics_) {
        renderMemorySection(target);
        renderCameraSection(target);
    }
}

void DebugPanel::resize(const sf::FloatRect& area) 
{
    area_ = area;
    float scaleFactor = area.height / 600.0f;
    padding_ = 10.0f * scaleFactor;
    lineHeight_ = 16.0f * scaleFactor;
    sectionSpacing_ = 20.0f * scaleFactor;
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    titleText_.setCharacterSize(static_cast<unsigned int>(14 * scaleFactor));
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
    for (auto& text : metricTexts_) {
        text.setCharacterSize(static_cast<unsigned int>(12 * scaleFactor));
    }
    float graphWidth = graphBackground_.getSize().x;
    graphBackground_.setPosition(area_.left + area_.width - graphWidth - padding_,
                                area_.top + padding_ + 30.0f * scaleFactor);
}

void DebugPanel::setVisible(bool visible) 
{
    visible_ = visible;
}

bool DebugPanel::isVisible() const 
{
    return visible_;
}

void DebugPanel::toggle() 
{
    visible_ = !visible_;
}

void DebugPanel::updateFPS(float fps, float frameTime)
{
    metrics_.currentFPS = fps;
    metrics_.frameTime = frameTime;
    float sum = 0.0f;
    for (float f : fpsHistory_) {
        sum += f;
    }
    metrics_.averageFPS = sum / static_cast<float>(fpsHistory_.size());
}

void DebugPanel::updateRenderTime(float renderTime) 
{
    metrics_.renderTime = renderTime;
}

void DebugPanel::updateNetworkStats(float latency, size_t messagesPerSec, size_t bytesPerSec) 
{
    metrics_.networkLatency = latency;
    metrics_.messagesPerSecond = messagesPerSec;
    metrics_.bytesPerSecond = bytesPerSec;
}

void DebugPanel::setShowAll(bool show) 
{
    showAllMetrics_ = show;
}

void DebugPanel::calculateMetrics(const World& world, const Camera& camera, const NetworkClient& network) 
{
    (void)camera;
    (void)network;
    metrics_.entityCount = world.getPlayerCount() + world.getEggCount();
    metrics_.visibleEntities = metrics_.entityCount;
    metrics_.particleCount = 0;
    metrics_.memoryUsage = metrics_.entityCount * 1024;
    metrics_.gpuMemoryUsage = 0;
}

void DebugPanel::renderBackground(sf::RenderTarget& target) 
{
    target.draw(background_);
    target.draw(border_);
    target.draw(titleText_);
}

void DebugPanel::renderPerformanceSection(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + titleText_.getGlobalBounds().height + padding_;
    size_t textIndex = 0;
    std::ostringstream oss;
    oss << "FPS: " << std::fixed << std::setprecision(1) << metrics_.currentFPS
        << " (avg: " << metrics_.averageFPS << ")";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Frame: " << std::fixed << std::setprecision(2) << metrics_.frameTime << "ms";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Render: " << std::fixed << std::setprecision(2) << metrics_.renderTime << "ms";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    for (size_t i = 0; i < textIndex; ++i) {
        target.draw(metricTexts_[i]);
    }
    target.draw(graphBackground_);
    target.draw(fpsGraph_);
}

void DebugPanel::renderEntitySection(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + titleText_.getGlobalBounds().height + padding_ + (3 * lineHeight_) + sectionSpacing_;
    size_t textIndex = 3;
    std::ostringstream oss;
    oss << "Entities: " << metrics_.entityCount;
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Visible: " << metrics_.visibleEntities;
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Particles: " << metrics_.particleCount;
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    for (size_t i = 3; i < textIndex; ++i) {
        target.draw(metricTexts_[i]);
    }
}

void DebugPanel::renderNetworkSection(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + titleText_.getGlobalBounds().height + padding_ + (6 * lineHeight_) + (2 * sectionSpacing_);
    size_t textIndex = 6;
    std::ostringstream oss;
    oss << "Latency: " << std::fixed << std::setprecision(1) << metrics_.networkLatency << "ms";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Msg/s: " << metrics_.messagesPerSecond;
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "KB/s: " << (metrics_.bytesPerSecond / 1024);
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    for (size_t i = 6; i < textIndex; ++i) {
        target.draw(metricTexts_[i]);
    }
}

void DebugPanel::renderMemorySection(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + titleText_.getGlobalBounds().height + padding_ + (9 * lineHeight_) + (3 * sectionSpacing_);
    size_t textIndex = 9;
    std::ostringstream oss;
    oss << "Memory: " << (metrics_.memoryUsage / 1024) << " KB";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "GPU Memory: " << (metrics_.gpuMemoryUsage / 1024) << " KB";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    for (size_t i = 9; i < textIndex; ++i) {
        target.draw(metricTexts_[i]);
    }
}

void DebugPanel::renderCameraSection(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + titleText_.getGlobalBounds().height + padding_ + (11 * lineHeight_) + (4 * sectionSpacing_);
    size_t textIndex = 11;
    std::ostringstream oss;
    oss << "Camera Info";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    y += lineHeight_;
    oss.str("");
    oss << "Position: (0, 0)";
    updateText(metricTexts_[textIndex++], oss.str(), area_.left + padding_, y);
    for (size_t i = 11; i < textIndex; ++i) {
        target.draw(metricTexts_[i]);
    }
}

void DebugPanel::updateText(sf::Text& text, const std::string& content, float x, float y) 
{
    text.setString(content);
    text.setPosition(x, y);
}

}