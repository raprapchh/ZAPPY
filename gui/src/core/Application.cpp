#include "Application.hpp"
#include "Config.hpp"
#include "model/World.hpp"
#include "network/NetworkClient.hpp"
#include "utils/ResourceManager.hpp"
#include "utils/Math.hpp"
#include "view/Renderer3D.hpp"

#ifdef RAYLIB_AVAILABLE
#include <raylib.h>
#else
#define KEY_ESCAPE 256
#define KEY_F3 292
[[maybe_unused]] static bool IsKeyPressed(int)
{ 
  return false;
}
#endif

namespace zappy {
Application::Application(const Config& config)
    : eventDispatcher_(std::make_unique<EventDispatcher>())
    , config_(config)
    , running_(false)
    , initialized_(false)
    , targetFrameTime_(1.0f / 60.0f)
    , frameTime_(0.0f)
    , renderTime_(0.0f)
    , updateTime_(0.0f)
{
    LOG_INFO("Application created");
}

Application::~Application() 
{
    shutdown();
    LOG_INFO("Application destroyed");
}

bool Application::initialize() 
{
    LOG_INFO("Initializing application...");
    Logger::getInstance().setLogToConsole(true);
    
    if (config_.renderMode == Config::RenderMode::Mode2D) {
        sf::VideoMode videoMode(config_.windowWidth, config_.windowHeight);
        std::string title = "Zappy GUI";
        window_.create(videoMode, title, sf::Style::Default | sf::Style::Resize);
        window_.setVerticalSyncEnabled(config_.vsync);
        if (config_.fpsLimit > 0) {
            window_.setFramerateLimit(config_.fpsLimit);
            targetFrameTime_ = 1.0f / config_.fpsLimit;
        }
        LOG_INFOF("2D Window created: %dx%d", config_.windowWidth, config_.windowHeight);
    } else {
        LOG_INFO("3D mode selected - window will be managed by Raylib");
    }
    if (!ResourceManager::getInstance().loadAll()) {
        LOG_ERROR("Failed to load resources");
        return false;
    }
    if (!music_.openFromFile("gui/assets/music/playtime.mp3")) {
        LOG_ERROR("Failed to load music file: gui/assets/music/playtime.mp3");
        return false;
    }
    music_.setLoop(true);
    music_.play();

    if (!deathSoundBuffer_.loadFromFile("gui/assets/music/get-out.mp3")) {
        LOG_ERROR("Failed to load death sound file: gui/assets/music/get-out.mp3");
        return false;
    }
    deathSound_.setBuffer(deathSoundBuffer_);
    if (!initializeSystems()) {
        LOG_ERROR("Failed to initialize systems");
        return false;
    }
    initialized_ = true;
    LOG_INFO("Application initialized successfully");
    return true;
}

void Application::run() 
{
    if (!initialized_) {
        LOG_ERROR("Application not initialized");
        return;
    }
    LOG_INFO("Starting main loop");
    running_ = true;
    deltaClock_.restart();
    frameClock_.restart();
    
    if (config_.renderMode == Config::RenderMode::Mode2D) {
        while (running_ && window_.isOpen()) {
            float deltaTime = deltaClock_.restart().asSeconds();
            deltaTime = std::min(deltaTime, 1.0f / 30.0f);
            performanceClock_.restart();
            handleEvents();
            updateTime_ = performanceClock_.restart().asSeconds();
            update(deltaTime);
            renderTime_ = performanceClock_.restart().asSeconds();
            render();
            frameTime_ = frameClock_.restart().asSeconds();
        }
    } else {
        while (running_ && renderer_ && renderer_->isWindowReady() && !renderer_->shouldClose()) {
            float deltaTime = deltaClock_.restart().asSeconds();
            deltaTime = std::min(deltaTime, 1.0f / 30.0f);
            performanceClock_.restart();
            
            renderer_->handleEvents();
            handle3DEvents();
            
            updateTime_ = performanceClock_.restart().asSeconds();
            update(deltaTime);
            renderTime_ = performanceClock_.restart().asSeconds();
            render();
            frameTime_ = frameClock_.restart().asSeconds();
        }
    }
    LOG_INFO("Main loop ended");
}

void Application::shutdown() 
{
    if (!initialized_) return;
    LOG_INFO("Shutting down application...");
    running_ = false;
    shutdownSystems();
    
    if (config_.renderMode == Config::RenderMode::Mode2D && window_.isOpen()) {
        window_.close();
    }
    
    ResourceManager::getInstance().unloadAll();
    initialized_ = false;
    LOG_INFO("Application shut down complete");
}

void Application::handleEvents() 
{
    if (config_.renderMode != Config::RenderMode::Mode2D) {
        return;
    }
    sf::Event event;
    while (window_.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                onWindowClosed();
                break;
            case sf::Event::Resized:
                onWindowResized(event.size.width, event.size.height);
                break;
            case sf::Event::KeyPressed:
                onKeyPressed(event.key.code);
                break;
            case sf::Event::MouseButtonPressed:
                onMouseButtonPressed(event.mouseButton.button,
                                   event.mouseButton.x, event.mouseButton.y);
                break;
            case sf::Event::MouseMoved:
                onMouseMoved(event.mouseMove.x, event.mouseMove.y);
                break;
            case sf::Event::MouseWheelScrolled:
                onMouseWheelScrolled(event.mouseWheelScroll.delta,
                                   event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                break;
            default:
                break;
        }
        if (ui_) {
            ui_->handleEvent(event);
        }
    }
}

void Application::update(float deltaTime) 
{
    if (network_) {
        while (network_->hasMessages()) {
            auto message = network_->getNextMessage();
            if (message.has_value()) {
                if (world_) {
                    processMessage(*message);
                }
            }
        }
    }
    if (world_) {
        world_->update(deltaTime);
    }
    if (camera_) {
        camera_->update(deltaTime);
    }
    if (ui_) {
        ui_->update(deltaTime, *world_);
        auto& debugPanel = ui_->getDebugPanel();
        debugPanel.updateFPS(1.0f / frameTime_, frameTime_);
        debugPanel.updateRenderTime(renderTime_);
        if (network_) {
        }
    }
}

void Application::render() 
{
    if (config_.renderMode == Config::RenderMode::Mode2D) {
        window_.clear(sf::Color(50, 150, 50));
        if (world_ && camera_ && renderer_) {
            renderer_->render(*world_, *camera_, frameTime_);
        }
        if (ui_) {
            ui_->render(window_);
        }
        window_.display();
    } else {
        if (world_ && camera_ && renderer_) {
            renderer_->render(*world_, *camera_, frameTime_);
        }
    }
}

bool Application::initializeSystems() 
{
    LOG_INFO("Initializing systems...");
    world_ = std::make_unique<World>(*eventDispatcher_);
    LOG_INFO("World initialized");
    camera_ = std::make_unique<Camera>();
    camera_->setViewSize(static_cast<float>(config_.windowWidth),
                        static_cast<float>(config_.windowHeight));
    camera_->setMode(CameraMode::OVERVIEW);
    LOG_INFO("Camera initialized in OVERVIEW mode");
    if (config_.renderMode == Config::RenderMode::Mode3D) {
        LOG_INFO("Creating 3D renderer...");
#ifdef RAYLIB_AVAILABLE
        if (config_.fullscreen) {
            SetConfigFlags(FLAG_FULLSCREEN_MODE);
        }
#endif
        renderer_ = std::make_unique<Renderer3D>(config_, ui_.get());
    } else {
        LOG_INFO("Creating 2D renderer...");
        renderer_ = std::make_unique<Renderer>(window_, *eventDispatcher_);
    }
    
    if (!renderer_->initialize()) {
        LOG_ERROR("Failed to initialize renderer");
        return false;
    }
    LOG_INFO("Renderer initialized");

    if (config_.renderMode == Config::RenderMode::Mode2D) {
        ui_ = std::make_unique<UIManager>(config_.showFPS);
        if (!ui_->initialize(window_)) {
            LOG_ERROR("Failed to initialize UI manager");
            return false;
        }
        LOG_INFO("UI manager initialized");
    } else {
        LOG_INFO("Skipping UI manager initialization for 3D mode");
    }
    network_ = std::make_unique<NetworkClient>(config_);
    if (!network_->connect()) {
        LOG_ERROR("Failed to connect to server");
        return false;
    }
    LOG_INFO("Network client initialized");
    return true;
}

void Application::shutdownSystems() 
{
    LOG_INFO("Shutting down systems...");
    network_.reset();
    ui_.reset();
    renderer_.reset();
    camera_.reset();
    world_.reset();
    eventDispatcher_.reset();
    LOG_INFO("Systems shut down");
}

void Application::onWindowClosed() 
{
    LOG_INFO("Window close requested");
    running_ = false;
}

void Application::onWindowResized(unsigned int width, unsigned int height) 
{
    LOG_INFOF("Window resized: %dx%d", width, height);
    sf::FloatRect visibleArea(0, 0, static_cast<float>(width), static_cast<float>(height));
    window_.setView(sf::View(visibleArea));
    if (renderer_) {
        renderer_->resize(width, height);
    }
    if (camera_) {
        camera_->setViewSize(static_cast<float>(width), static_cast<float>(height));
    }
    if (ui_) {
        ui_->onWindowResized(width, height);
    }
}

void Application::onKeyPressed(sf::Keyboard::Key key)
{
    if (ui_ && ui_->handleKeyPress(key)) {
        return;
    }
    switch (key) {
        case sf::Keyboard::Escape:
            running_ = false;
            break;
        case sf::Keyboard::I:
            if (ui_ && world_) {
                auto& playerInfoPanel = ui_->getPlayerInfoPanel();
                if (playerInfoPanel.isVisible()) {
                    PlayerID currentId = playerInfoPanel.getPlayerId();
                    const auto& players = world_->getPlayers();
                    if (players.size() > 1) {
                        auto it = players.find(currentId);
                        if (it != players.end()) {
                            it++;
                            if (it == players.end()) {
                                it = players.begin();
                            }
                            ui_->showPlayerInfoPanel(it->first, *it->second);
                        }
                    }
                } else {
                    const auto& players = world_->getPlayers();
                    if (!players.empty()) {
                        ui_->showPlayerInfoPanel(players.begin()->first, *players.begin()->second);
                    }
                }
            }
            break;
        case sf::Keyboard::F3:
            if (ui_) {
                ui_->toggleDebugPanel();
            }
            break;
        case sf::Keyboard::F11:
            break;
        case sf::Keyboard::Tab:
            if (camera_) {
                CameraMode currentMode = camera_->getMode();
                CameraMode nextMode = static_cast<CameraMode>(
                    (static_cast<int>(currentMode) + 1) % 4);
                camera_->setMode(nextMode);
            }
            break;
        case sf::Keyboard::Space:
            if (camera_) {
                camera_->resetView();
            }
            break;
        default:
            break;
    }
    if (camera_) {
        camera_->onKeyPressed(key);
    }
}

void Application::onMouseButtonPressed(sf::Mouse::Button button, int x, int y) 
{
    if (camera_) {
        camera_->onMouseButtonPressed(button, x, y);
    }
    if (ui_ && button == sf::Mouse::Left) {
        
        if (world_ && camera_) {
            sf::Vector2f worldCoords = camera_->screenToWorld(x, y);
            sf::Vector2i tileCoords = zappy::math::worldToTile(worldCoords.x, worldCoords.y, constants::TILE_SIZE);

            bool playerClicked = false;
            for (const auto& pair : world_->getPlayers()) {
                const Player* player = pair.second.get();
                if (player && player->getBounds().contains(worldCoords)) {
                    ui_->showPlayerInfoPanel(player->getId(), *player);
                    playerClicked = true;
                    break;
                }
            }

            if (!playerClicked) {
                if (world_->isValidPosition(tileCoords.x, tileCoords.y)) {
                    const Tile& clickedTile = world_->getTile(tileCoords.x, tileCoords.y);
                    ui_->showTileInfoPanel(tileCoords.x, tileCoords.y, clickedTile);
                }
            }
        }
    }
}

void Application::onMouseMoved(int x, int y) 
{
    if (camera_) {
        camera_->onMouseMoved(x, y);
    }
}

void Application::onMouseWheelScrolled(float delta, int , int ) 
{
    if (camera_) {
        camera_->onMouseWheelScrolled(delta);
    }
}

void Application::handle3DEvents() 
{
    if (config_.renderMode != Config::RenderMode::Mode3D) {
        return;
    }
    
#ifdef RAYLIB_AVAILABLE
    if (IsKeyPressed(KEY_ESCAPE)) {
        running_ = false;
    }
    
    if (IsKeyPressed(KEY_F3)) {
        if (renderer_) {
            renderer_->setShowDebug(!renderer_->isWindowReady());
        }
    }
#endif
}

void Application::processMessage(const Message& message) 
{
    LOG_DEBUGF("Received message: %s", message.toString().c_str());
    switch (message.type) {
        case MessageType::MapSize: {
            if (message.hasMinParams(2)) {
                auto width = message.getIntParam(0);
                auto height = message.getIntParam(1);
                if (width && height) {
                    world_->setMapSize(*width, *height);
                    if (camera_) {
                        camera_->setBounds(0.0f, 0.0f, *width * 32.0f, *height * 32.0f);
                        camera_->resetView();
                        LOG_DEBUGF("Camera bounds set and view reset for map %dx%d (%.1f x %.1f pixels)",
                                  *width, *height, *width * 32.0f, *height * 32.0f);
                    }
                }
            }
            break;
        }
        case MessageType::TileContent: {
            if (message.hasMinParams(9)) {
                auto x = message.getIntParam(0);
                auto y = message.getIntParam(1);
                if (x && y && world_->isValidPosition(*x, *y)) {
                    Tile& tile = world_->getTile(*x, *y);
                    std::array<int, 7> resources;
                    for (int i = 0; i < 7; ++i) {
                        auto quantity = message.getIntParam(2 + i);
                        resources[i] = quantity ? *quantity : 0;
                    }
                    tile.setResources(resources);
                }
            }
            break;
        }
        case MessageType::TeamName: {
            if (message.hasMinParams(1)) {
                std::string teamName = message.getParam<std::string>(0).value();
                TeamID newTeamId = static_cast<TeamID>(world_->getTeams().size() + 1);
                if (!world_->getTeamByName(teamName)) {
                    world_->addTeam(std::make_unique<Team>(newTeamId, teamName));
                    LOG_INFOF("Added team: %s", teamName.c_str());
                }
            }
            break;
        }
        case MessageType::PlayerNew: {
            if (message.hasMinParams(6)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                auto x = message.getIntParam(1);
                auto y = message.getIntParam(2);
                auto orientation = message.getIntParam(3);
                auto level = message.getIntParam(4);
                std::string teamName = message.getParam<std::string>(5).value();
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        LOG_ERROR("Failed to parse player ID: " + playerIdStr);
                        break;
                    }
                }
                if (playerId >= 0 && x && y && orientation && level) {
                    Team* team = world_->getTeamByName(teamName);
                    if (!team) {
                        TeamID newTeamId = static_cast<TeamID>(world_->getTeams().size() + 1);
                        world_->addTeam(std::make_unique<Team>(newTeamId, teamName));
                        team = world_->getTeamByName(teamName);
                    }
                    auto player = std::make_unique<Player>(playerId, teamName, *x, *y,
                                                         static_cast<PlayerOrientation>(*orientation));
                    player->setLevel(*level);
                    if (team) {
                        player->setTeamId(team->getId());
                    }
                    world_->addPlayer(std::move(player));
                    LOG_INFOF("Added player %d at (%d, %d) for team %s", playerId, *x, *y, teamName.c_str());
                }
            }
            break;
        }
        case MessageType::PlayerPosition: {
            if (message.hasMinParams(4)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                auto x = message.getIntParam(1);
                auto y = message.getIntParam(2);
                auto orientation = message.getIntParam(3);
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        break;
                    }
                }
                if (playerId >= 0 && x && y && orientation) {
                    Player* player = world_->getPlayer(playerId);
                    if (player) {
                        player->setServerPosition(*x, *y);
                        player->setOrientation(static_cast<PlayerOrientation>(*orientation));
                    } else {
                        LOG_WARNINGF("Received PPO for unknown player %d", playerId);
                    }
                }
            }
            break;
        }
        case MessageType::PlayerLevel: {
            if (message.hasMinParams(2)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                auto level = message.getIntParam(1);
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        break;
                    }
                }
                if (playerId >= 0 && level) {
                    Player* player = world_->getPlayer(playerId);
                    if (player) {
                        player->setLevel(*level);
                    }
                }
            }
            break;
        }
        case MessageType::PlayerInventory: {
            if (message.hasMinParams(10)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                auto x = message.getIntParam(1);
                auto y = message.getIntParam(2);
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        break;
                    }
                }
                if (playerId >= 0 && x && y) {
                    Player* player = world_->getPlayer(playerId);
                    if (player) {
                        player->setServerPosition(*x, *y);
                        std::array<int, 7> inventory;
                        for (int i = 0; i < 7; ++i) {
                            auto quantity = message.getIntParam(3 + i);
                            inventory[i] = quantity ? *quantity : 0;
                        }
                        player->setInventory(inventory);
                    }
                }
            }
            break;
        }
        case MessageType::PlayerDeath: {
            if (message.hasMinParams(1)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        break;
                    }
                }
                if (playerId >= 0) {
                    Player* player = world_->getPlayer(playerId);
                    if (player) {
                        sf::Vector2f pos = player->getPosition();
                        sf::Vector2i tilePos(static_cast<int>(pos.x / 32.0f), static_cast<int>(pos.y / 32.0f));
                        renderer_->triggerDeathAnimation(tilePos);
                        
                        player->setAlive(false);
                        world_->removePlayer(playerId);
                        deathSound_.play();
                        LOG_INFOF("Player %d died. Playing death sound.", playerId);
                    }
                }
            }
            break;
        }
        case MessageType::PlayerBroadcast: {
            if (message.hasMinParams(2)) {
                std::string playerIdStr = message.getParam<std::string>(0).value();
                std::string broadcastMessage = message.getParam<std::string>(1).value();
                int playerId = -1;
                if (playerIdStr.length() > 1 && playerIdStr[0] == '#') {
                    try {
                        playerId = std::stoi(playerIdStr.substr(1));
                    } catch (const std::exception&) {
                        break;
                    }
                }
                if (playerId >= 0) {
                    Player* player = world_->getPlayer(playerId);
                    if (player) {
                        sf::Vector2f pos = player->getPosition();
                        sf::Vector2i tilePos(static_cast<int>(pos.x / 32), static_cast<int>(pos.y / 32));
                        float intensity = std::min(1.0f + broadcastMessage.length() / 50.0f, 3.0f);
                        
                        if (renderer_) {
                            renderer_->triggerBroadcastAnimation(tilePos, intensity, player->getTeamColor());
                        }
                        
                        player->startBroadcast(broadcastMessage);
                        LOG_INFOF("Player %d broadcasting: %s", playerId, broadcastMessage.c_str());
                    }
                }
            }
            break;
        }
        case MessageType::IncantationEnd: {
            if (message.hasMinParams(3)) {
                auto x = message.getIntParam(0);
                auto y = message.getIntParam(1);
                auto success = message.getIntParam(2);
                
                if (x && y && success) {
                    sf::Vector2i tilePos(*x, *y);
                    
                    if (*success == 1) {
                        const auto& players = world_->getPlayers();
                        for (const auto& pair : players) {
                            const Player* player = pair.second.get();
                            if (player) {
                                sf::Vector2f playerPos = player->getPosition();
                                sf::Vector2i playerTile(static_cast<int>(playerPos.x / 32), static_cast<int>(playerPos.y / 32));
                                
                                if (playerTile.x == tilePos.x && playerTile.y == tilePos.y) {
                                    if (renderer_) {
                                        renderer_->triggerElevationAnimation(tilePos, player->getLevel(), player->getTeamColor());
                                    }
                                    LOG_INFOF("Elevation animation triggered at (%d, %d) for player %d level %d", 
                                            tilePos.x, tilePos.y, player->getId(), player->getLevel());
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}
}
