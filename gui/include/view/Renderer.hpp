#pragma once
#include "IRenderer.hpp"
#include "Common.hpp"
#include "model/World.hpp"
#include "view/Camera.hpp"
#include "view/SpriteManager.hpp"
#include "view/ParticleSystem.hpp"
#include "view/TileAnimationSystem.hpp"
#include "view/Animation.hpp"
#include "utils/ResourceManager.hpp"
#include "events/EventDispatcher.hpp"
namespace zappy {
class Renderer : public IRenderer {
public:
    explicit Renderer(sf::RenderWindow& window, EventDispatcher& dispatcher);
    ~Renderer();

    bool initialize() override;
    void shutdown() override;
    void render(const World& world, const Camera& camera, float deltaTime) override;
    void resize(unsigned int width, unsigned int height) override;

    void renderTiles(const World& world, const Camera& camera);
    void renderResources(const World& world, const Camera& camera);
    void renderEntities(const World& world, const Camera& camera, float deltaTime);
    void renderEffects(const Camera& camera, float deltaTime);
    void renderUI();
    void renderPlayerInfoPanel(const World& world);
    void renderResourceInfoPanel(const World& world);
    void renderDebugInfo(const World& world, const Camera& camera, float deltaTime);
    void renderGrid(const Camera& camera);

    void setQuality(int quality) override;
    void setShowGrid(bool show) override;
    void setShowDebug(bool show) override;
    bool isWindowReady() const override;
    bool shouldClose() const override;
    void handleEvents() override;
    void triggerElevationAnimation(sf::Vector2i position, int level, const sf::Color& teamColor) override;
    void triggerBroadcastAnimation(sf::Vector2i position, float intensity, const sf::Color& teamColor) override;
    void triggerDeathAnimation(sf::Vector2i position) override;

private:
    void setupRenderStates();
    void calculateVisibleBounds(const Camera& camera);
    void subscribeToEvents();
    void onPlayerAdded(const PlayerAddedEvent& event);
    void onPlayerRemoved(const PlayerRemovedEvent& event);
    bool isVisible(const sf::FloatRect& bounds) const;
    std::vector<size_t> getVisibleTiles(const World& world) const;
    std::vector<size_t> getVisibleEntities(const World& world) const;
    void batchSprites();
    void flushBatches();

    sf::RenderWindow& window_;
    EventDispatcher& dispatcher_;
    std::unique_ptr<SpriteManager> spriteManager_;
    std::unique_ptr<ParticleSystem> particleSystem_;
    std::unique_ptr<TileAnimationSystem> tileAnimationSystem_;
    std::unique_ptr<AnimationSystem> animationSystem_;
    sf::FloatRect visibleBounds_;
    sf::View worldView_;
    sf::View uiView_;
    int quality_;
    bool showGrid_;
    bool showDebug_;
    std::vector<size_t> visibleTileCache_;
    std::vector<size_t> visibleEntityCache_;
    sf::Vector2f lastCameraPos_;
    bool cacheValid_;
    sf::Texture skyTexture_;
    sf::Sprite skySprite_;
    sf::Texture playerTexture_;
    sf::Sprite playerSprite_;
};
}
