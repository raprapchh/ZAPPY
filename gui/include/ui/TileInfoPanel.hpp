#pragma once
#include "Common.hpp"
#include "model/Tile.hpp"
#include "utils/ResourceManager.hpp"
namespace zappy {
class TileInfoPanel {
public:
    TileInfoPanel();
    ~TileInfoPanel() = default;

    bool initialize(const sf::FloatRect& area);
    void update(float deltaTime);
    void render(sf::RenderTarget& target);
    void resize(const sf::FloatRect& area);

    void setVisible(bool visible);
    bool isVisible() const;
    void toggle();

    void showTileInfo(int x, int y, const Tile& tile);

private:
    void renderBackground(sf::RenderTarget& target);
    void renderContent(sf::RenderTarget& target);
    void updateTextPositions();

    sf::FloatRect area_;
    bool visible_;
    sf::Vector2i tilePosition_;
    std::array<int, 7> resources_;
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::Font font_;
    sf::Text titleText_;
    sf::Text coordsText_;
    std::vector<sf::Text> resourceTexts_;
    const std::array<std::string, 7> resourceNames_ = {
        "Food", "Linemate", "Deraumere", "Sibur", "Mendiane", "Phiras", "Thystame"
    };
    std::map<ResourceType, sf::Color> resourceColors_;
    sf::Color backgroundColor_;
    sf::Color borderColor_;
    sf::Color textColor_;
    float padding_;
    float lineHeight_;
};
}
