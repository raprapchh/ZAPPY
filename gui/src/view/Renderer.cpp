#include "view/Renderer.hpp"
#include "view/TileAnimationSystem.hpp"
#include "events/Event.hpp"
#include "utils/Logger.hpp"
#include "utils/Math.hpp"
#include <algorithm>

namespace zappy {

Renderer::Renderer(sf::RenderWindow& window, EventDispatcher& dispatcher)
    : window_(window)
    , dispatcher_(dispatcher)
    , spriteManager_(nullptr)
    , particleSystem_(nullptr)
    , tileAnimationSystem_(nullptr)
    , animationSystem_(nullptr)
    , visibleBounds_(0, 0, 0, 0)
    , quality_(1)
    , showGrid_(false)
    , showDebug_(false)
    , lastCameraPos_(0, 0)
    , cacheValid_(false)
    , skyTexture_()
    , skySprite_()
{
    subscribeToEvents();
}

Renderer::~Renderer()
{
    shutdown();
}

bool Renderer::initialize() 
{
    try {
        spriteManager_ = std::make_unique<SpriteManager>();
        particleSystem_ = std::make_unique<ParticleSystem>();
        tileAnimationSystem_ = std::make_unique<TileAnimationSystem>();
        animationSystem_ = std::make_unique<AnimationSystem>();
        if (!skyTexture_.loadFromFile("gui/assets/sprites/sky.png")) {
            return false;
        }
        skySprite_.setTexture(skyTexture_);
        sf::Vector2u windowSize = window_.getSize();
        float scaleX = static_cast<float>(windowSize.x) / skyTexture_.getSize().x;
        float scaleY = static_cast<float>(windowSize.y) / skyTexture_.getSize().y;
        float scale = std::max(scaleX, scaleY);
        skySprite_.setScale(scale, scale);
        sf::FloatRect spriteBounds = skySprite_.getLocalBounds();
        skySprite_.setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);
        skySprite_.setPosition(static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f);
        if (!playerTexture_.loadFromFile("gui/assets/sprites/animation.png")) {
            return false;
        }
        playerSprite_.setTexture(playerTexture_);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Renderer::render(const World& world, const Camera& camera, float deltaTime) 
{
    sf::View uiView(sf::FloatRect(0, 0, static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y)));
    window_.setView(uiView);
    window_.draw(skySprite_);
    calculateVisibleBounds(camera);
    window_.setView(camera.getView());
    renderTiles(world, camera);
    renderResources(world, camera);
    
    if (tileAnimationSystem_) {
        tileAnimationSystem_->update(deltaTime);
        tileAnimationSystem_->render(window_, camera.getView());
    }
    
    renderEntities(world, camera, deltaTime);
    renderEffects(camera, deltaTime);
    window_.setView(uiView);
    renderUI();
    renderPlayerInfoPanel(world);
    renderResourceInfoPanel(world);
    if (showDebug_) {
        renderDebugInfo(world, camera, deltaTime);
    }
    if (showGrid_) {
        renderGrid(camera);
    }
}

void Renderer::resize(unsigned int width, unsigned int height) 
{
    sf::Vector2u windowSize(width, height);
    float scaleX = static_cast<float>(windowSize.x) / skyTexture_.getSize().x;
    float scaleY = static_cast<float>(windowSize.y) / skyTexture_.getSize().y;
    float scale = std::max(scaleX, scaleY);
    skySprite_.setScale(scale, scale);
    sf::FloatRect spriteBounds = skySprite_.getLocalBounds();
    skySprite_.setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);
    skySprite_.setPosition(static_cast<float>(windowSize.x) / 2.0f, static_cast<float>(windowSize.y) / 2.0f);
}
void Renderer::renderTiles(const World& world, const Camera& ) {
    sf::Vector2i mapSize = world.getMapSize();
    if (ResourceManager::getInstance().hasTexture("grass")) {
        const sf::Texture& grassTexture = ResourceManager::getInstance().getTexture("grass");
        sf::Sprite grassSprite(grassTexture);
        for (int y = 0; y < mapSize.y; ++y) {
            for (int x = 0; x < mapSize.x; ++x) {
                grassSprite.setPosition(static_cast<float>(x * 32), static_cast<float>(y * 32));
                sf::Vector2u textureSize = grassTexture.getSize();
                float scaleX = 32.0f / static_cast<float>(textureSize.x);
                float scaleY = 32.0f / static_cast<float>(textureSize.y);
                grassSprite.setScale(scaleX, scaleY);
                window_.draw(grassSprite);
                sf::RectangleShape border(sf::Vector2f(32.0f, 32.0f));
                border.setPosition(static_cast<float>(x * 32), static_cast<float>(y * 32));
                border.setFillColor(sf::Color::Transparent);
                border.setOutlineColor(sf::Color::Blue);
                border.setOutlineThickness(1.0f);
                window_.draw(border);
            }
        }
    } else {
        sf::RectangleShape tileShape(sf::Vector2f(32.0f, 32.0f));
        tileShape.setFillColor(sf::Color(50, 150, 50));
        tileShape.setOutlineColor(sf::Color(40, 120, 40));
        tileShape.setOutlineThickness(0.5f);
        for (int y = 0; y < mapSize.y; ++y) {
            for (int x = 0; x < mapSize.x; ++x) {
                tileShape.setPosition(static_cast<float>(x * 32), static_cast<float>(y * 32));
                window_.draw(tileShape);
            }
        }
    }
}

void Renderer::renderResources(const World& world, const Camera& ) 
{
    sf::Vector2i mapSize = world.getMapSize();
    for (int y = 0; y < mapSize.y; ++y) {
        for (int x = 0; x < mapSize.x; ++x) {
            const Tile& tile = world.getTile(x, y);
            const auto& resources = tile.getResources();
            std::vector<ResourceType> availableResources;
            for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
                if (resources[i] > 0) {
                    availableResources.push_back(static_cast<ResourceType>(i));
                }
            }
            if (availableResources.empty()) continue;
            sf::Vector2f tilePos(static_cast<float>(x * 32), static_cast<float>(y * 32));
            for (size_t i = 0; i < availableResources.size(); ++i) {
                ResourceType type = availableResources[i];
                int quantity = resources[static_cast<int>(type)];
                if (quantity > 0) {
                    float offsetX, offsetY;
                    if (availableResources.size() == 1) {
                        offsetX = 16.0f;
                        offsetY = 16.0f;
                    } else if (availableResources.size() == 2) {
                        offsetX = (i == 0) ? 8.0f : 24.0f;
                        offsetY = 16.0f;
                    } else if (availableResources.size() <= 4) {
                        offsetX = ((i % 2) == 0) ? 8.0f : 24.0f;
                        offsetY = (i < 2) ? 8.0f : 24.0f;
                    } else {
                        offsetX = (i % 3) * 10.0f + 6.0f;
                        offsetY = (i / 3) * 10.0f + 6.0f;
                    }
                    std::string textureName;
                    switch (type) {
                        case ResourceType::Food:
                            textureName = "food";
                            break;
                        case ResourceType::Linemate:
                            textureName = "crystal_grey";
                            break;
                        case ResourceType::Deraumere:
                            textureName = "crystal_blue";
                            break;
                        case ResourceType::Sibur:
                            textureName = "crystal_green";
                            break;
                        case ResourceType::Mendiane:
                            textureName = "crystal_pink";
                            break;
                        case ResourceType::Phiras:
                            textureName = "crystal_yellow";
                            break;
                        case ResourceType::Thystame:
                            textureName = "crystal_orange";
                            break;
                        default:
                            textureName = "";
                            break;
                    }
                    if (!textureName.empty() && ResourceManager::getInstance().hasTexture(textureName)) {
                        const sf::Texture& crystalTexture = ResourceManager::getInstance().getTexture(textureName);
                        sf::Sprite crystalSprite(crystalTexture);
                        crystalSprite.setPosition(tilePos.x + offsetX - 6.0f, tilePos.y + offsetY - 6.0f);
                        sf::Vector2u textureSize = crystalTexture.getSize();
                        float scale = 12.0f / static_cast<float>(std::max(textureSize.x, textureSize.y));
                        crystalSprite.setScale(scale, scale);
                        window_.draw(crystalSprite);
                    } else {
                        sf::CircleShape resourceShape(6.0f);
                        resourceShape.setPosition(tilePos.x + offsetX - 6.0f, tilePos.y + offsetY - 6.0f);
                        switch (type) {
                            case ResourceType::Food:
                                resourceShape.setFillColor(sf::Color(255, 255, 0));
                                break;
                            case ResourceType::Linemate:
                                resourceShape.setFillColor(sf::Color(255, 255, 255));
                                break;
                            case ResourceType::Deraumere:
                                resourceShape.setFillColor(sf::Color(0, 100, 255));
                                break;
                            case ResourceType::Sibur:
                                resourceShape.setFillColor(sf::Color(0, 255, 100));
                                break;
                            case ResourceType::Mendiane:
                                resourceShape.setFillColor(sf::Color(255, 0, 255));
                                break;
                            case ResourceType::Phiras:
                                resourceShape.setFillColor(sf::Color(255, 255, 0));
                                break;
                            case ResourceType::Thystame:
                                resourceShape.setFillColor(sf::Color(255, 100, 0));
                                break;
                            default:
                                resourceShape.setFillColor(sf::Color::White);
                                break;
                        }
                        resourceShape.setOutlineColor(sf::Color(0, 0, 0, 100));
                        resourceShape.setOutlineThickness(0.5f);
                        window_.draw(resourceShape);
                    }
                }
            }
        }
    }
}

void Renderer::renderEntities(const World& world, const Camera& , float ) 
{
    auto visibleEntities = getVisibleEntities(world);
    const auto& players = world.getPlayers();
    for (const auto& pair : players) {
        const Player* player = pair.second.get();
        if (!player || !player->isAlive()) continue;
        sf::Vector2f pos = player->getInterpolatedPosition();
        sf::Vector2f centerPos(pos.x + 16.0f, pos.y + 16.0f);
        sf::FloatRect playerBounds(centerPos.x - 16, centerPos.y - 16, 32, 32);
        if (!isVisible(playerBounds)) continue;
        playerSprite_.setPosition(centerPos.x - 16.0f, centerPos.y - 16.0f);
        int frameWidth = 33;
        int frameHeight = 47;
        int row = 0;
        int col = 0;
        if (player->isInIncantation()) {
            row = 0;
            col = 6 + (player->getCurrentFrame() % 3);
        } else {
            switch (player->getOrientation()) {
                case PlayerOrientation::NORTH:
                    row = 2;
                    break;
                case PlayerOrientation::EAST:
                    row = 1;
                    break;
                case PlayerOrientation::SOUTH:
                    row = 0;
                    break;
                case PlayerOrientation::WEST:
                    row = 3;
                    break;
            }
            col = player->getCurrentFrame();
            col = std::max(0, std::min(col, 2));
        }
        playerSprite_.setTextureRect(sf::IntRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight));
        playerSprite_.setColor(player->getTeamColor());
        playerSprite_.setScale(32.0f / frameWidth, 32.0f / frameHeight);
        window_.draw(playerSprite_);
    }
    const auto& eggs = world.getEggs();
    for (const auto& pair : eggs) {
        const Egg* egg = pair.second.get();
        if (!egg) continue;
        sf::Vector2f pos = egg->getPosition();
        sf::FloatRect eggBounds(pos.x - 8, pos.y - 8, 16, 16);
        if (!isVisible(eggBounds)) continue;
        sf::CircleShape eggShape(6.0f);
        eggShape.setPosition(pos.x - 6.0f, pos.y - 6.0f);
        eggShape.setFillColor(sf::Color(255, 255, 200));
        eggShape.setOutlineColor(sf::Color::Black);
        eggShape.setOutlineThickness(1.0f);
        window_.draw(eggShape);
    }
}

void Renderer::renderEffects(const Camera& camera, float deltaTime) 
{
    if (particleSystem_) {
        particleSystem_->update(deltaTime);
        particleSystem_->render(window_, camera.getView());
    }
}

void Renderer::renderUI() 
{
}

void Renderer::renderDebugInfo(const World& world, const Camera& camera, float ) 
{
    sf::View uiView(sf::FloatRect(0, 0, static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y)));
    window_.setView(uiView);
    sf::Font font;
    sf::Text debugText;
    debugText.setCharacterSize(14);
    debugText.setFillColor(sf::Color::White);
    if (font.loadFromFile("gui/assets/fonts/Display.ttf")) {
        debugText.setFont(font);
    }
    std::string debugInfo = "Debug Info:\n";
    debugInfo += "Camera: (" + std::to_string(camera.getPosition().x) + ", " + std::to_string(camera.getPosition().y) + ")\n";
    debugInfo += "Zoom: " + std::to_string(camera.getZoom()) + "\n";
    debugInfo += "Players: " + std::to_string(world.getPlayers().size()) + "\n";
    debugInfo += "Eggs: " + std::to_string(world.getEggs().size());
    debugText.setString(debugInfo);
    debugText.setPosition(10, 10);
    window_.draw(debugText);
}

void Renderer::renderGrid(const Camera& camera) 
{
    sf::View view = camera.getView();
    window_.setView(view);
    sf::FloatRect viewBounds = camera.getViewBounds();
    for (int x = static_cast<int>(viewBounds.left / 32) * 32; x < viewBounds.left + viewBounds.width; x += 32) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(static_cast<float>(x), viewBounds.top), sf::Color(255, 255, 255, 50)),
            sf::Vertex(sf::Vector2f(static_cast<float>(x), viewBounds.top + viewBounds.height), sf::Color(255, 255, 255, 50))
        };
        window_.draw(line, 2, sf::Lines);
    }
    for (int y = static_cast<int>(viewBounds.top / 32) * 32; y < viewBounds.top + viewBounds.height; y += 32) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(viewBounds.left, static_cast<float>(y)), sf::Color(255, 255, 255, 50)),
            sf::Vertex(sf::Vector2f(viewBounds.left + viewBounds.width, static_cast<float>(y)), sf::Color(255, 255, 255, 50))
        };
        window_.draw(line, 2, sf::Lines);
    }
}

void Renderer::setQuality(int quality) 
{
    quality_ = std::clamp(quality, 0, 3);
}

void Renderer::setShowGrid(bool show)
{
    showGrid_ = show;
}

void Renderer::setShowDebug(bool show) 
{
    showDebug_ = show;
}

void Renderer::shutdown() 
{
}

bool Renderer::isWindowReady() const 
{
    return window_.isOpen();
}

bool Renderer::shouldClose() const 
{
    return !window_.isOpen();
}

void Renderer::handleEvents() 
{
}

void Renderer::triggerElevationAnimation(sf::Vector2i position, int level, const sf::Color& teamColor) {
    if (tileAnimationSystem_) {
        tileAnimationSystem_->startElevationAnimation(position, level, teamColor);
    }
    
    if (particleSystem_) {
        float worldX = position.x * 32.0f + 16.0f;
        float worldY = position.y * 32.0f + 16.0f;
        particleSystem_->emitElevation(worldX, worldY, level);
    }
}

void Renderer::triggerBroadcastAnimation(sf::Vector2i position, float intensity, const sf::Color& teamColor) {
    if (tileAnimationSystem_) {
        tileAnimationSystem_->startBroadcastWave(position, intensity, teamColor);
    }
}

void Renderer::triggerDeathAnimation(sf::Vector2i position) {
    if (tileAnimationSystem_) {
        tileAnimationSystem_->startDeathAnimation(position);
    }
}

void Renderer::subscribeToEvents() 
{
    dispatcher_.subscribe<PlayerAddedEvent>([this](const PlayerAddedEvent& event) { this->onPlayerAdded(event); });
    dispatcher_.subscribe<PlayerRemovedEvent>([this](const PlayerRemovedEvent& event) { this->onPlayerRemoved(event); });
}

void Renderer::onPlayerAdded(const PlayerAddedEvent& ) 
{
    cacheValid_ = false;
}

void Renderer::onPlayerRemoved(const PlayerRemovedEvent& ) 
{
    cacheValid_ = false;
}

void Renderer::renderPlayerInfoPanel(const World& world) 
{
    sf::View uiView(sf::FloatRect(0, 0, static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y)));
    window_.setView(uiView);
    sf::Font font;
    if (!font.loadFromFile("gui/assets/fonts/Display.ttf")) {
        return;
    }
    const auto& players = world.getPlayers();
    if (players.empty()) return;
    float panelWidth = 300.0f;
    float panelHeight = 70.0f * players.size() + 70.0f;
    float panelX = window_.getSize().x - panelWidth - 15.0f;
    float panelY = 15.0f;
    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2.0f);
    window_.draw(panel);
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setString("Players Info");
    titleText.setPosition(panelX + 20.0f, panelY + 10.0f);
    window_.draw(titleText);
    int index = 0;
    for (const auto& pair : players) {
        const Player* player = pair.second.get();
        if (!player) continue;
        float yPos = panelY + 60.0f + index * 70.0f;
        sf::CircleShape colorIndicator(15.0f);
        colorIndicator.setPosition(panelX + 20.0f, yPos + 10.0f);
        colorIndicator.setFillColor(player->getTeamColor());
        colorIndicator.setOutlineColor(sf::Color::White);
        colorIndicator.setOutlineThickness(2.0f);
        window_.draw(colorIndicator);
        sf::Text playerText;
        playerText.setFont(font);
        playerText.setCharacterSize(28);
        playerText.setFillColor(sf::Color::White);
        std::string playerInfo = "ID:" + std::to_string(player->getId()) +
                                " Lvl:" + std::to_string(player->getLevel()) +
                                " Food:" + std::to_string(player->getResource(ResourceType::Food));
        playerText.setString(playerInfo);
        playerText.setPosition(panelX + 55.0f, yPos + 10.0f);
        window_.draw(playerText);
        index++;
    }
}

void Renderer::renderResourceInfoPanel(const World& world)
{
    sf::View uiView(sf::FloatRect(0, 0, static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y)));
    window_.setView(uiView);
    sf::Font font;
    if (!font.loadFromFile("gui/assets/fonts/Display.ttf")) {
        return;
    }

    std::map<ResourceType, int> totalResources;
    const std::vector<ResourceType> gemTypes = {
        ResourceType::Linemate, ResourceType::Deraumere, ResourceType::Sibur,
        ResourceType::Mendiane, ResourceType::Phiras, ResourceType::Thystame
    };

    for (const auto& type : gemTypes) {
        totalResources[type] = 0;
    }

    sf::Vector2i mapSize = world.getMapSize();
    if (mapSize.x > 0 && mapSize.y > 0) {
        for (int y = 0; y < mapSize.y; ++y) {
            for (int x = 0; x < mapSize.x; ++x) {
                const Tile& tile = world.getTile(x, y);
                for (const auto& type : gemTypes) {
                    totalResources[type] += tile.getResource(type);
                }
            }
        }
    }

    std::map<ResourceType, sf::Color> resourceColors;
    resourceColors[ResourceType::Linemate] = sf::Color(128, 128, 128);
    resourceColors[ResourceType::Deraumere] = sf::Color(0, 100, 255);
    resourceColors[ResourceType::Sibur] = sf::Color(0, 200, 0);
    resourceColors[ResourceType::Mendiane] = sf::Color(255, 105, 180);
    resourceColors[ResourceType::Phiras] = sf::Color(255, 165, 0);
    resourceColors[ResourceType::Thystame] = sf::Color(128, 0, 128);
    std::map<ResourceType, std::string> resourceNames;
    resourceNames[ResourceType::Linemate] = "Linemate";
    resourceNames[ResourceType::Deraumere] = "Deraumere";
    resourceNames[ResourceType::Sibur] = "Sibur";
    resourceNames[ResourceType::Mendiane] = "Mendiane";
    resourceNames[ResourceType::Phiras] = "Phiras";
    resourceNames[ResourceType::Thystame] = "Thystame";

    const auto& players = world.getPlayers();
    std::vector<const Player*> alivePlayers;
    for (const auto& pair : players) {
        if (pair.second && pair.second->isAlive()) {
            alivePlayers.push_back(pair.second.get());
        }
    }

    float playerPanelHeight = 0;
    if (!alivePlayers.empty()) {
        playerPanelHeight = 70.0f * alivePlayers.size() + 70.0f;
    }

    float panelWidth = 300.0f;
    float panelHeight = 60.0f + 50.0f * 6;
    float panelX = window_.getSize().x - panelWidth - 15.0f;
    float panelY = 15.0f + playerPanelHeight + 20.0f;
    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2.0f);
    window_.draw(panel);
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setString("Map Resources");
    titleText.setPosition(panelX + 20.0f, panelY + 10.0f);
    window_.draw(titleText);

    for (size_t i = 0; i < gemTypes.size(); ++i) {
        ResourceType type = gemTypes[i];
        float yPos = panelY + 60.0f + i * 50.0f;
        sf::CircleShape colorIndicator(12.0f);
        colorIndicator.setPosition(panelX + 20.0f, yPos + 8.0f);
        colorIndicator.setFillColor(resourceColors[type]);
        colorIndicator.setOutlineColor(sf::Color::White);
        colorIndicator.setOutlineThickness(2.0f);
        window_.draw(colorIndicator);
        sf::Text resourceText;
        resourceText.setFont(font);
        resourceText.setCharacterSize(22);
        resourceText.setFillColor(sf::Color::White);
        std::string resourceInfo = resourceNames[type] + ": " + std::to_string(totalResources[type]);
        resourceText.setString(resourceInfo);
        resourceText.setPosition(panelX + 50.0f, yPos + 5.0f);
        window_.draw(resourceText);
    }
}

void Renderer::setupRenderStates() 
{
}

void Renderer::calculateVisibleBounds(const Camera& camera) 
{
    visibleBounds_ = camera.getViewBounds();
    sf::Vector2f currentPos = camera.getPosition();
    if (math::length(currentPos - lastCameraPos_) > 32.0f) {
        cacheValid_ = false;
        lastCameraPos_ = currentPos;
    }
}

bool Renderer::isVisible(const sf::FloatRect& bounds) const 
{
    return visibleBounds_.intersects(bounds);
}

std::vector<size_t> Renderer::getVisibleTiles(const World& world) const {
    (void)world;
    std::vector<size_t> visibleTiles;
    return visibleTiles;
}

std::vector<size_t> Renderer::getVisibleEntities(const World& world) const 
{
    (void)world;
    std::vector<size_t> visibleEntities;
    return visibleEntities;
}

void Renderer::batchSprites() 
{
}

void Renderer::flushBatches() 
{
}

}