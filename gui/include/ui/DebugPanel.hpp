#pragma once
#include "Common.hpp"
#include "model/World.hpp"
#include "view/Camera.hpp"
#include "network/NetworkClient.hpp"
namespace zappy {
struct PerformanceMetrics {
    float currentFPS;
    float averageFPS;
    float frameTime;
    float renderTime;
    float updateTime;
    float networkTime;
    size_t entityCount;
    size_t visibleEntities;
    size_t particleCount;
    size_t memoryUsage;
    size_t gpuMemoryUsage;
    float networkLatency;
    size_t messagesPerSecond;
    size_t bytesPerSecond;
};
class DebugPanel {
public:
    DebugPanel();
    ~DebugPanel() = default;

    bool initialize(const sf::FloatRect& area);
    void update(float deltaTime, const World& world, const Camera& camera,
               const NetworkClient& network);
    void render(sf::RenderTarget& target);
    void resize(const sf::FloatRect& area);

    void setVisible(bool visible);
    bool isVisible() const;
    void toggle();

    void updateFPS(float fps, float frameTime);
    void updateRenderTime(float renderTime);
    void updateNetworkStats(float latency, size_t messagesPerSec, size_t bytesPerSec);
    void setShowAll(bool show);

private:
    void calculateMetrics(const World& world, const Camera& camera, const NetworkClient& network);
    void renderBackground(sf::RenderTarget& target);
    void renderPerformanceSection(sf::RenderTarget& target);
    void renderEntitySection(sf::RenderTarget& target);
    void renderNetworkSection(sf::RenderTarget& target);
    void renderMemorySection(sf::RenderTarget& target);
    void renderCameraSection(sf::RenderTarget& target);
    void updateText(sf::Text& text, const std::string& content, float x, float y);

    sf::FloatRect area_;
    bool visible_;
    bool showAllMetrics_;
    PerformanceMetrics metrics_;
    std::vector<float> fpsHistory_;
    size_t fpsHistoryIndex_;
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::RectangleShape sectionSeparator_;
    sf::Font font_;
    sf::Text titleText_;
    std::vector<sf::Text> metricTexts_;
    sf::VertexArray fpsGraph_;
    sf::RectangleShape graphBackground_;
    sf::Color backgroundColor_;
    sf::Color borderColor_;
    sf::Color textColor_;
    sf::Color graphColor_;
    sf::Color separatorColor_;
    float padding_;
    float lineHeight_;
    float sectionSpacing_;
    size_t maxFPSHistory_;
};
}
