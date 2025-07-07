#pragma once
#include "Common.hpp"
#include "model/Player.hpp"
#include "utils/ResourceManager.hpp"

namespace zappy {
class PlayerInfoPanel {
public:
    PlayerInfoPanel();
    ~PlayerInfoPanel() = default;

    bool initialize(const sf::FloatRect& area);
    void update(float deltaTime, const World& world);
    void render(sf::RenderTarget& target);
    void resize(const sf::FloatRect& area);

    void setVisible(bool visible);
    bool isVisible() const;
    void toggle();

    void showPlayerInfo(PlayerID id, const Player& player);
    PlayerID getPlayerId() const;

private:
    void renderBackground(sf::RenderTarget& target);
    void renderContent(sf::RenderTarget& target);
    void updateTextPositions();

    sf::FloatRect area_;
    bool visible_;
    PlayerID playerId_;
    const Player* currentPlayer_ = nullptr;
    std::array<int, 7> inventory_;
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::Font font_;
    sf::Text titleText_;
    sf::Text idText_;
    sf::Text levelText_;
    sf::Text teamText_;
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
