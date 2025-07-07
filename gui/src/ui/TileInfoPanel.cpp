#include "ui/TileInfoPanel.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace zappy {
TileInfoPanel::TileInfoPanel()
    : visible_(false)
    , tilePosition_({-1, -1})
    , resources_({0})
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

bool TileInfoPanel::initialize(const sf::FloatRect& area)
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
    titleText_.setString("Tile Information");
    titleText_.setCharacterSize(22);
    titleText_.setFillColor(textColor_);
    
    coordsText_.setFont(font_);
    coordsText_.setCharacterSize(22);
    coordsText_.setFillColor(textColor_);
    
    for (auto& text : resourceTexts_) {
        text.setFont(font_);
        text.setCharacterSize(22);
        text.setFillColor(textColor_);
    }
    
    updateTextPositions();
    LOG_DEBUG("TileInfoPanel initialized");
    return true;
}

void TileInfoPanel::update(float deltaTime)
{
    (void)deltaTime;
}

void TileInfoPanel::render(sf::RenderTarget& target)
{
    if (!visible_) return;
    renderBackground(target);
    renderContent(target);
}

void TileInfoPanel::resize(const sf::FloatRect& area)
{
    area_ = area;
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    updateTextPositions();
}

void TileInfoPanel::setVisible(bool visible)
{
    visible_ = visible;
}

bool TileInfoPanel::isVisible() const
{
    return visible_;
}

void TileInfoPanel::toggle()
{
    visible_ = !visible_;
}

void TileInfoPanel::showTileInfo(int x, int y, const Tile& tile)
{
    tilePosition_ = {x, y};
    resources_ = tile.getResources();
    visible_ = true;
    updateTextPositions();
}

void TileInfoPanel::renderBackground(sf::RenderTarget& target)
{
    target.draw(background_);
    target.draw(border_);
    target.draw(titleText_);
}

void TileInfoPanel::renderContent(sf::RenderTarget& target)
{
    target.draw(coordsText_);
    float currentY = area_.top + 70.0f;
    for (size_t i = 0; i < resourceNames_.size() && i < resources_.size(); ++i) {
        
        sf::CircleShape colorIndicator(12.0f);
        colorIndicator.setPosition(area_.left + padding_, currentY + 4.0f);
        colorIndicator.setFillColor(resourceColors_[static_cast<ResourceType>(i)]);
        colorIndicator.setOutlineColor(sf::Color::White);
        colorIndicator.setOutlineThickness(1.0f);
        target.draw(colorIndicator);

        target.draw(resourceTexts_[i]);
        currentY += lineHeight_;
    }
}

void TileInfoPanel::updateTextPositions()
{
    
    titleText_.setPosition(area_.left + padding_, area_.top + 10.0f);
    
    std::ostringstream oss;
    oss << "Coords: (" << tilePosition_.x << ", " << tilePosition_.y << ")";
    coordsText_.setString(oss.str());
    coordsText_.setPosition(area_.left + padding_, area_.top + 40.0f);
    
    float currentY = area_.top + 70.0f;
    for (size_t i = 0; i < resourceNames_.size() && i < resources_.size(); ++i) {
        oss.str("");
        oss << resourceNames_[i] << ": " << resources_[i];
        resourceTexts_[i].setString(oss.str());
        resourceTexts_[i].setPosition(area_.left + padding_ + 30.0f, currentY);
        currentY += lineHeight_;
    }
}

}