#pragma once
#include "Common.hpp"
namespace zappy {
enum class EventType {
    PLAYER_CONNECT,
    PLAYER_DISCONNECT,
    PLAYER_DEATH,
    PLAYER_ELEVATION,
    PLAYER_BROADCAST,
    INCANTATION_START,
    INCANTATION_END,
    EGG_LAID,
    EGG_HATCHED,
    RESOURCE_DROP,
    TEAM_WIN,
    SYSTEM_INFO,
    ERROR
};
struct GameEvent {
    EventType type;
    std::string message;
    float timestamp;
    sf::Color color;
    PlayerID playerId;
    TeamID teamId;
    GameEvent(EventType t, const std::string& msg, float time = 0.0f,
             sf::Color c = sf::Color::White, PlayerID pid = 0, TeamID tid = 0)
        : type(t), message(msg), timestamp(time), color(c), playerId(pid), teamId(tid) {}
};
class EventLog {
public:
    EventLog();
    ~EventLog() = default;

    bool initialize(const sf::FloatRect& area);
    void update(float deltaTime);
    void render(sf::RenderTarget& target);
    void resize(const sf::FloatRect& area);

    void addEvent(const GameEvent& event);
    void addPlayerConnect(PlayerID playerId, const std::string& teamName);
    void addPlayerDisconnect(PlayerID playerId, const std::string& teamName);
    void addPlayerDeath(PlayerID playerId, const std::string& teamName);
    void addPlayerElevation(PlayerID playerId, const std::string& teamName, int newLevel);
    void addBroadcast(PlayerID playerId, const std::string& message);
    void addIncantationStart(int x, int y, int level);
    void addIncantationEnd(int x, int y, bool success);
    void addEggLaid(TeamID teamId, int x, int y);
    void addEggHatched(TeamID teamId, int x, int y);
    void addTeamWin(const std::string& teamName);
    void addSystemInfo(const std::string& message);
    void addError(const std::string& message);

    void setVisible(bool visible);
    bool isVisible() const;
    void setMaxEvents(size_t maxEvents);
    void setShowTimestamps(bool show);
    void setAutoScroll(bool autoScroll);

    void scrollUp();
    void scrollDown();
    void scrollToTop();
    void scrollToBottom();
    void clear();

private:
    void renderBackground(sf::RenderTarget& target);
    void renderEvents(sf::RenderTarget& target);
    void renderScrollbar(sf::RenderTarget& target);
    void updateScrollPosition();
    void cleanupOldEvents();
    sf::Color getEventColor(EventType type) const;
    std::string formatTimestamp(float timestamp) const;

    sf::FloatRect area_;
    bool visible_;
    std::deque<GameEvent> events_;
    size_t maxEvents_;
    size_t scrollPosition_;
    size_t visibleLines_;
    sf::RectangleShape background_;
    sf::RectangleShape border_;
    sf::RectangleShape scrollbar_;
    sf::RectangleShape scrollThumb_;
    sf::Font font_;
    sf::Text titleText_;
    std::vector<sf::Text> eventTexts_;
    bool showTimestamps_;
    bool autoScroll_;
    float lineHeight_;
    float padding_;
    sf::Color backgroundColor_;
    sf::Color borderColor_;
    sf::Color scrollbarColor_;
    sf::Color scrollThumbColor_;
    std::map<EventType, sf::Color> eventColors_;
    float gameTime_;
};
}
