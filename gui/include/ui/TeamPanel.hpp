#pragma once
#include "Common.hpp"
#include "model/World.hpp"
#include "model/Team.hpp"
namespace zappy {
struct TeamDisplayInfo {
    std::string name;
    sf::Color color;
    std::array<int, 8> levelCounts;
    std::array<int, 7> resources;
    int playerCount;
    int eggCount;
    float victoryProgress;
    bool isWinning;
};
class TeamPanel {
public:
    TeamPanel();
    ~TeamPanel() = default;

    bool initialize(const sf::FloatRect& area);
    void update(float deltaTime, const World& world);
    void render(sf::RenderTarget& target);
    void resize(const sf::FloatRect& area);

    void setVisible(bool visible);
    bool isVisible() const;
    void setSelectedTeam(const std::string& teamName);
    void clearSelection();

private:
    void renderBackground(sf::RenderTarget& target);
    void renderTeamList(sf::RenderTarget& target);

    sf::FloatRect area_;
    bool visible_;
    std::vector<TeamDisplayInfo> teams_;
    std::string selectedTeam_;
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::RectangleShape teamBackground_;
    sf::RectangleShape progressBar_;
    sf::RectangleShape progressFill_;
    sf::Font font_;
    sf::Text titleText_;
    sf::Text teamNameText_;
    sf::Text statsText_;
    sf::Text levelText_;
    sf::Text resourceText_;
    sf::Color backgroundColor_;
    sf::Color borderColor_;
    sf::Color textColor_;
    sf::Color progressBackColor_;
    sf::Color winningTeamColor_;
    float padding_;
    float lineHeight_;
    float sectionSpacing_;
};
}
