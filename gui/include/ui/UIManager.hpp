#pragma once
#include "Common.hpp"
#include "ui/HUD.hpp"

#include "ui/TeamPanel.hpp"
#include "ui/DebugPanel.hpp"
#include "ui/EventLog.hpp"
#include "ui/TileInfoPanel.hpp"
#include "ui/PlayerInfoPanel.hpp"
namespace zappy {
class UIManager {
public:
    explicit UIManager(bool showFPS = false);
    ~UIManager() = default;

    bool initialize(sf::RenderWindow& window);
    void update(float deltaTime, const World& world);
    void render(sf::RenderTarget& target);
    void handleEvent(const sf::Event& event);
    void onWindowResized(unsigned int width, unsigned int height);

    void toggleDebugPanel();
    void toggleTeamPanel();

    void showNotification(const std::string& message, float duration = 3.0f);
    void showTileInfoPanel(int x, int y, const Tile& tile);
    void showPlayerInfoPanel(PlayerID id, const Player& player);

    void setShowFPS(bool show);
    void setUIScale(float scale);

    HUD& getHUD();
    
    TeamPanel& getTeamPanel();
    DebugPanel& getDebugPanel();
    EventLog& getEventLog();
    TileInfoPanel& getTileInfoPanel();
    PlayerInfoPanel& getPlayerInfoPanel();

    bool handleKeyPress(sf::Keyboard::Key key);
    void handleMouseClick(sf::Mouse::Button button);

private:
    void setupLayout();
    void updateLayout();

    std::unique_ptr<HUD> hud_;
    
    std::unique_ptr<TeamPanel> teamPanel_;
    std::unique_ptr<DebugPanel> debugPanel_;
    std::unique_ptr<EventLog> eventLog_;
    std::unique_ptr<TileInfoPanel> tileInfoPanel_;
    std::unique_ptr<PlayerInfoPanel> playerInfoPanel_;

    sf::RenderWindow* window_;
    sf::Vector2u windowSize_;
    bool showDebug_;
    
    bool showTeamPanel_;
    bool showFPS_;
    float uiScale_;

    sf::FloatRect hudArea_;
    
    sf::FloatRect teamPanelArea_;
    sf::FloatRect debugPanelArea_;
    sf::FloatRect eventLogArea_;
    sf::FloatRect tileInfoPanelArea_;
};
}
