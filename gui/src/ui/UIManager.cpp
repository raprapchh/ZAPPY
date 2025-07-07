#include "ui/UIManager.hpp"
#include "utils/Logger.hpp"
#include "view/Camera.hpp"

namespace zappy {

UIManager::UIManager(bool showFPS)
    : window_(nullptr)
    , windowSize_(800, 600)
    , showDebug_(false)
    , showTeamPanel_(false)
    , showFPS_(showFPS)
    , uiScale_(1.0f)
{
}

bool UIManager::initialize(sf::RenderWindow& window) 
{
    window_ = &window;
    windowSize_ = window.getSize();
    hud_ = std::make_unique<HUD>(showFPS_);
    if (!hud_->initialize(windowSize_)) {
        LOG_ERROR("Failed to initialize HUD");
        return false;
    }
    
    teamPanel_ = std::make_unique<TeamPanel>();
    debugPanel_ = std::make_unique<DebugPanel>();
    eventLog_ = std::make_unique<EventLog>();
    tileInfoPanel_ = std::make_unique<TileInfoPanel>();
    sf::FloatRect tileInfoPanelArea(30, 100, 300, 360);
    if (!tileInfoPanel_->initialize(tileInfoPanelArea)) {
        LOG_ERROR("Failed to initialize TileInfoPanel");
        return false;
    }
    playerInfoPanel_ = std::make_unique<PlayerInfoPanel>();
    sf::FloatRect playerInfoPanelArea(30, 100, 300, 360);
    if (!playerInfoPanel_->initialize(playerInfoPanelArea)) {
        LOG_ERROR("Failed to initialize PlayerInfoPanel");
        return false;
    }
    setupLayout();
    LOG_INFO("UIManager initialized successfully");
    return true;
}

void UIManager::update(float deltaTime, const World& world) 
{
    if (hud_) {
        hud_->update(deltaTime, world);
    }
    
    if (tileInfoPanel_) {
        tileInfoPanel_->update(deltaTime);
    }
    if (playerInfoPanel_) {
        playerInfoPanel_->update(deltaTime, world);
    }
}

void UIManager::render(sf::RenderTarget& target) 
{
    if (hud_) {
        hud_->render(target);
    }
    
    if (tileInfoPanel_) {
        tileInfoPanel_->render(target);
    }
    if (playerInfoPanel_) {
        playerInfoPanel_->render(target);
    }
}

void UIManager::handleEvent(const sf::Event& event) 
{
    switch (event.type) {
        case sf::Event::KeyPressed:
            handleKeyPress(event.key.code);
            break;
        case sf::Event::MouseButtonPressed:
            handleMouseClick(event.mouseButton.button);
            break;
        default:
            break;
    }
}

void UIManager::onWindowResized(unsigned int width, unsigned int height) 
{
    windowSize_ = sf::Vector2u(width, height);
    if (hud_) {
        hud_->resize(windowSize_);
    }
    
    if (tileInfoPanel_) {
        sf::FloatRect tileInfoPanelArea(30, 100, 300, 300);
        tileInfoPanel_->resize(tileInfoPanelArea);
    }
    if (playerInfoPanel_) {
        sf::FloatRect playerInfoPanelArea(30, 100, 300, 360);
        playerInfoPanel_->resize(playerInfoPanelArea);
    }
    setupLayout();
}

void UIManager::toggleDebugPanel() 
{
    if (debugPanel_) {
        debugPanel_->setVisible(!debugPanel_->isVisible());
    }
}



void UIManager::toggleTeamPanel() 
{
    if (teamPanel_) {
        teamPanel_->setVisible(!teamPanel_->isVisible());
    }
}

void UIManager::showNotification(const std::string& message, float duration) 
{
    LOG_INFO("Notification: " + message + " (duration: " + std::to_string(duration) + "s)");
}

void UIManager::showTileInfoPanel(int x, int y, const Tile& tile) 
{
    if (playerInfoPanel_ && playerInfoPanel_->isVisible()) {
        playerInfoPanel_->setVisible(false);
    }
    if (tileInfoPanel_) {
        tileInfoPanel_->showTileInfo(x, y, tile);
    }
}

void UIManager::showPlayerInfoPanel(PlayerID id, const Player& player) 
{
    if (tileInfoPanel_ && tileInfoPanel_->isVisible()) {
        tileInfoPanel_->setVisible(false);
    }
    if (playerInfoPanel_) {
        playerInfoPanel_->showPlayerInfo(id, player);
    }
}

void UIManager::setShowFPS(bool show) 
{
    showFPS_ = show;
}

void UIManager::setUIScale(float scale) 
{
    uiScale_ = std::max(0.5f, std::min(2.0f, scale));
    updateLayout();
}

HUD& UIManager::getHUD() 
{
    return *hud_;
}



TeamPanel& UIManager::getTeamPanel() 
{
    return *teamPanel_;
}

DebugPanel& UIManager::getDebugPanel() 
{
    return *debugPanel_;
}

EventLog& UIManager::getEventLog() 
{
    return *eventLog_;
}

TileInfoPanel& UIManager::getTileInfoPanel() 
{
    return *tileInfoPanel_;
}

PlayerInfoPanel& UIManager::getPlayerInfoPanel() 
{
    return *playerInfoPanel_;
}

bool UIManager::handleKeyPress(sf::Keyboard::Key key) 
{
    if (tileInfoPanel_ && tileInfoPanel_->isVisible()) {
        if (key == sf::Keyboard::Escape) {
            tileInfoPanel_->setVisible(false);
            return true;
        }
    }
    if (playerInfoPanel_ && playerInfoPanel_->isVisible()) {
        if (key == sf::Keyboard::Escape) {
            playerInfoPanel_->setVisible(false);
            return true;
        }
    }
    switch (key) {
        case sf::Keyboard::F1:
            toggleDebugPanel();
            return true;
        
        case sf::Keyboard::T:
            toggleTeamPanel();
            return true;
        default:
            break;
    }
    return false;
}

void UIManager::handleMouseClick(sf::Mouse::Button button) 
{
    if (button == sf::Mouse::Left) {
        
    }
}

void UIManager::setupLayout() 
{
    updateLayout();
}

void UIManager::updateLayout() 
{
}

}
