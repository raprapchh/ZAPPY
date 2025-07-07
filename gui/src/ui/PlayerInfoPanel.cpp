#include "ui/PlayerInfoPanel.hpp"
#include "utils/Logger.hpp"
#include "model/World.hpp"
#include <sstream>
#include <iomanip>

namespace zappy {

PlayerInfoPanel::PlayerInfoPanel()
    : visible_(false)
    , playerId_(-1)
    , inventory_({0})
    , backgroundColor_(0, 0, 0, 180)
    , borderColor_(255, 255, 255, 255)
    , textColor_(255, 255, 255, 255)
    , padding_(25.0f)
    , lineHeight_(30.0f)
{
    resourceTexts_.resize(resourceNames_.size());
    resourceColors_[ResourceType::Food] = sf::Color(139, 69, 19);
    resourceColors_[ResourceType::Linemate] = sf::Color(128, 128, 128);
    resourceColors_[ResourceType::Deraumere] = sf::Color(0, 100, 255);
    resourceColors_[ResourceType::Sibur] = sf::Color(0, 200, 0);
    resourceColors_[ResourceType::Mendiane] = sf::Color(255, 105, 180);
    resourceColors_[ResourceType::Phiras] = sf::Color(255, 165, 0);
    resourceColors_[ResourceType::Thystame] = sf::Color(128, 0, 128);
}

bool PlayerInfoPanel::initialize(const sf::FloatRect& area)
{
    area_ = area;
    if (!font_.loadFromFile("gui/assets/fonts/Display.ttf")) {
        LOG_ERROR("Failed to load font: gui/assets/fonts/Display.ttf");
        return false;
    }
    
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    background_.setFillColor(backgroundColor_);
    
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineThickness(2.0f);
    border_.setOutlineColor(borderColor_);
    
    titleText_.setFont(font_);
    titleText_.setString("Player Information");
    titleText_.setCharacterSize(22);
    titleText_.setFillColor(textColor_);
    
    idText_.setFont(font_);
    idText_.setCharacterSize(22);
    idText_.setFillColor(textColor_);

    levelText_.setFont(font_);
    levelText_.setCharacterSize(22);
    levelText_.setFillColor(textColor_);

    teamText_.setFont(font_);
    teamText_.setCharacterSize(22);
    teamText_.setFillColor(textColor_);
    
    for (auto& text : resourceTexts_) {
        text.setFont(font_);
        text.setCharacterSize(22);
        text.setFillColor(textColor_);
    }
    
    updateTextPositions();
    LOG_DEBUG("PlayerInfoPanel initialized");
    return true;
}

void PlayerInfoPanel::update(float deltaTime, const World& world)
{
    (void)deltaTime;
    const Player* playerInWorld = world.getPlayer(playerId_);

    if (!playerInWorld) {
        visible_ = false;
        currentPlayer_ = nullptr;
        return;
    }
    currentPlayer_ = playerInWorld;
    inventory_ = currentPlayer_->getInventory();
    titleText_.setString("Player " + std::to_string(playerId_) + " Information");
    idText_.setString("ID: " + std::to_string(playerId_));
    levelText_.setString("Level: " + std::to_string(currentPlayer_->getLevel()));
    teamText_.setString("Team: " + currentPlayer_->getTeamName());
    updateTextPositions();
}

void PlayerInfoPanel::render(sf::RenderTarget& target)
{
    if (!visible_) return;
    renderBackground(target);
    renderContent(target);
}

void PlayerInfoPanel::resize(const sf::FloatRect& area)
{
    area_ = area;
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    updateTextPositions();
}

void PlayerInfoPanel::setVisible(bool visible)
{
    visible_ = visible;
}

bool PlayerInfoPanel::isVisible() const
{
    return visible_;
}

void PlayerInfoPanel::toggle()
{
    visible_ = !visible_;
}

void PlayerInfoPanel::showPlayerInfo(PlayerID id, const Player& player)
{
    playerId_ = id;
    currentPlayer_ = &player;
    visible_ = true;
}

PlayerID PlayerInfoPanel::getPlayerId() const
{
    return playerId_;
}

void PlayerInfoPanel::renderBackground(sf::RenderTarget& target)
{
    target.draw(background_);
    target.draw(border_);
    target.draw(titleText_);
}

void PlayerInfoPanel::renderContent(sf::RenderTarget& target)
{
    target.draw(idText_);
    target.draw(levelText_);
    target.draw(teamText_);

    float currentY = area_.top + 130.0f;
    for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
        ResourceType type = static_cast<ResourceType>(i);
        sf::CircleShape colorIndicator(12.0f);
        colorIndicator.setPosition(area_.left + padding_, currentY + 4.0f);
        colorIndicator.setFillColor(resourceColors_[type]);
        colorIndicator.setOutlineColor(sf::Color::White);
        colorIndicator.setOutlineThickness(1.0f);
        target.draw(colorIndicator);

        target.draw(resourceTexts_[i]);
        currentY += lineHeight_;
    }
}

void PlayerInfoPanel::updateTextPositions()
{
    titleText_.setPosition(area_.left + padding_, area_.top + 10.0f);
    idText_.setPosition(area_.left + padding_, area_.top + 40.0f);
    levelText_.setPosition(area_.left + padding_, area_.top + 60.0f);
    teamText_.setPosition(area_.left + padding_, area_.top + 80.0f);

    float currentY = area_.top + 130.0f;
    for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
        ResourceType type = static_cast<ResourceType>(i);
        std::ostringstream oss;
        oss << resourceNames_[i] << ": " << inventory_[static_cast<int>(type)];
        resourceTexts_[i].setString(oss.str());
        resourceTexts_[i].setPosition(area_.left + padding_ + 30.0f, currentY);
        currentY += lineHeight_;
    }
}

}
