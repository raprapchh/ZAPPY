#include "ui/EventLog.hpp"
#include "utils/ResourceManager.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace zappy {

EventLog::EventLog()
    : area_(0, 0, 300, 400)
    , visible_(true)
    , maxEvents_(1000)
    , scrollPosition_(0)
    , visibleLines_(0)
    , showTimestamps_(true)
    , autoScroll_(true)
    , lineHeight_(20.0f)
    , padding_(10.0f)
    , backgroundColor_(sf::Color(0, 0, 0, 180))
    , borderColor_(sf::Color::White)
    , scrollbarColor_(sf::Color(100, 100, 100))
    , scrollThumbColor_(sf::Color(150, 150, 150))
    , gameTime_(0.0f) 
{
}

bool EventLog::initialize(const sf::FloatRect& area) 
{
    area_ = area;
    font_ = ResourceManager::getInstance().getFont("default");
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    background_.setFillColor(backgroundColor_);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineThickness(1.0f);
    border_.setOutlineColor(borderColor_);
    titleText_.setFont(font_);
    titleText_.setString("Event Log");
    titleText_.setCharacterSize(12);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
    scrollbar_.setSize(sf::Vector2f(8.0f, area_.height - 2 * padding_));
    scrollbar_.setPosition(area_.left + area_.width - padding_ - 8.0f, area_.top + padding_);
    scrollbar_.setFillColor(scrollbarColor_);
    LOG_DEBUG("EventLog initialized");
    return true;
}

void EventLog::update(float deltaTime) 
{
    (void)deltaTime;
    if (!visible_) return;
    gameTime_ += deltaTime;
    if (autoScroll_ && !events_.empty()) {
        float contentHeight = static_cast<float>(events_.size()) * lineHeight_;
        float visibleHeight = area_.height - 2 * padding_ - 20.0f;
        if (contentHeight > visibleHeight) {
            scrollPosition_ = static_cast<size_t>((contentHeight - visibleHeight) / lineHeight_);
        }
    }
}

void EventLog::render(sf::RenderTarget& target) 
{
    if (!visible_) return;
    renderBackground(target);
    renderEvents(target);
    renderScrollbar(target);
}

void EventLog::resize(const sf::FloatRect& area) 
{
    area_ = area;
    background_.setSize(sf::Vector2f(area_.width, area_.height));
    background_.setPosition(area_.left, area_.top);
    border_.setSize(sf::Vector2f(area_.width, area_.height));
    border_.setPosition(area_.left, area_.top);
    titleText_.setPosition(area_.left + padding_, area_.top + padding_);
    scrollbar_.setSize(sf::Vector2f(8.0f, area_.height - 2 * padding_));
    scrollbar_.setPosition(area_.left + area_.width - padding_ - 8.0f, area_.top + padding_);
}

void EventLog::addEvent(const GameEvent& event) 
{
    GameEvent newEvent = event;
    newEvent.timestamp = gameTime_;
    events_.push_back(newEvent);
    while (events_.size() > maxEvents_) {
        events_.pop_front();
    }
    if (autoScroll_) {
        float contentHeight = static_cast<float>(events_.size()) * lineHeight_;
        float visibleHeight = area_.height - 2 * padding_ - 20.0f;
        if (contentHeight > visibleHeight) {
            scrollPosition_ = static_cast<size_t>((contentHeight - visibleHeight) / lineHeight_);
        }
    }
}

void EventLog::clear() 
{
    events_.clear();
    scrollPosition_ = 0;
}

void EventLog::renderBackground(sf::RenderTarget& target) 
{
    target.draw(background_);
    target.draw(border_);
    target.draw(titleText_);
}

void EventLog::renderEvents(sf::RenderTarget& target) 
{
    float y = area_.top + padding_ + 20.0f;
    float visibleHeight = area_.height - 2 * padding_ - 20.0f;
    size_t startIndex = scrollPosition_;
    size_t endIndex = std::min(events_.size(), startIndex + static_cast<size_t>(visibleHeight / lineHeight_) + 1);
    for (size_t i = startIndex; i < endIndex; ++i) {
        const GameEvent& event = events_[i];
        float eventY = y + static_cast<float>(i - startIndex) * lineHeight_ -
                      (static_cast<float>(scrollPosition_) - static_cast<float>(startIndex) * lineHeight_);
        if (eventY < area_.top + padding_ + 20.0f ||
            eventY > area_.top + area_.height - padding_) {
            continue;
        }
        std::ostringstream oss;
        oss << "[" << std::fixed << std::setprecision(1) << event.timestamp << "s] ";
        sf::Text timestampText;
        timestampText.setFont(font_);
        timestampText.setString(oss.str());
        timestampText.setCharacterSize(10);
        timestampText.setFillColor(sf::Color(150, 150, 150, 255));
        timestampText.setPosition(area_.left + padding_, eventY);
        target.draw(timestampText);
        sf::Text messageText;
        messageText.setFont(font_);
        messageText.setString(event.message);
        messageText.setCharacterSize(10);
        messageText.setFillColor(event.color);
        float timestampWidth = timestampText.getLocalBounds().width;
        messageText.setPosition(area_.left + padding_ + timestampWidth + 5.0f, eventY);
        target.draw(messageText);
    }
}

void EventLog::renderScrollbar(sf::RenderTarget& target) 
{
    if (events_.empty()) return;
    float contentHeight = static_cast<float>(events_.size()) * lineHeight_;
    float visibleHeight = area_.height - 2 * padding_ - 20.0f;
    if (contentHeight <= visibleHeight) return;
    float thumbHeight = (visibleHeight / contentHeight) * scrollbar_.getSize().y;
    float thumbPosition = (static_cast<float>(scrollPosition_) / (contentHeight - visibleHeight)) *
                         (scrollbar_.getSize().y - thumbHeight);
    sf::RectangleShape thumb;
    thumb.setSize(sf::Vector2f(8.0f, thumbHeight));
    thumb.setPosition(scrollbar_.getPosition().x,
                     scrollbar_.getPosition().y + thumbPosition);
    thumb.setFillColor(scrollThumbColor_);
    target.draw(scrollbar_);
    target.draw(thumb);
}

sf::Color EventLog::getEventColor(EventType type) const 
{
    auto it = eventColors_.find(type);
    return (it != eventColors_.end()) ? it->second : sf::Color::White;
}

void EventLog::addPlayerConnect(PlayerID playerId, const std::string& teamName) 
{
    GameEvent event(EventType::PLAYER_CONNECT,
                   "Player " + std::to_string(playerId) + " from team " + teamName + " connected",
                   gameTime_, sf::Color::Green, playerId, 0);
    addEvent(event);
}

void EventLog::addPlayerDisconnect(PlayerID playerId, const std::string& teamName) 
{
    GameEvent event(EventType::PLAYER_DISCONNECT,
                   "Player " + std::to_string(playerId) + " from team " + teamName + " disconnected",
                   gameTime_, sf::Color::Red, playerId, 0);
    addEvent(event);
}

void EventLog::addPlayerDeath(PlayerID playerId, const std::string& teamName) 
{
    GameEvent event(EventType::PLAYER_DEATH,
                   "Player " + std::to_string(playerId) + " from team " + teamName + " died",
                   gameTime_, sf::Color(255, 100, 100), playerId, 0);
    addEvent(event);
}

void EventLog::addPlayerElevation(PlayerID playerId, const std::string& teamName, int newLevel) 
{
    GameEvent event(EventType::PLAYER_ELEVATION,
                   "Player " + std::to_string(playerId) + " from team " + teamName + " reached level " + std::to_string(newLevel),
                   gameTime_, sf::Color::Cyan, playerId, 0);
    addEvent(event);
}

void EventLog::addBroadcast(PlayerID playerId, const std::string& message) 
{
    GameEvent event(EventType::PLAYER_BROADCAST,
                   "Player " + std::to_string(playerId) + " broadcast: " + message,
                   gameTime_, sf::Color::Yellow, playerId, 0);
    addEvent(event);
}

void EventLog::addIncantationStart(int x, int y, int level) 
{
    GameEvent event(EventType::INCANTATION_START,
                   "Incantation level " + std::to_string(level) + " started at (" + std::to_string(x) + ", " + std::to_string(y) + ")",
                   gameTime_, sf::Color::Magenta, 0, 0);
    addEvent(event);
}

void EventLog::addIncantationEnd(int x, int y, bool success) 
{
    std::string result = success ? "succeeded" : "failed";
    GameEvent event(EventType::INCANTATION_END,
                   "Incantation at (" + std::to_string(x) + ", " + std::to_string(y) + ") " + result,
                   gameTime_, success ? sf::Color::Green : sf::Color::Red, 0, 0);
    addEvent(event);
}

void EventLog::addEggLaid(TeamID teamId, int x, int y) 
{
    GameEvent event(EventType::EGG_LAID,
                   "Egg laid by team " + std::to_string(teamId) + " at (" + std::to_string(x) + ", " + std::to_string(y) + ")",
                   gameTime_, sf::Color(255, 255, 100), 0, teamId);
    addEvent(event);
}

void EventLog::addEggHatched(TeamID teamId, int x, int y) 
{
    GameEvent event(EventType::EGG_HATCHED,
                   "Egg from team " + std::to_string(teamId) + " hatched at (" + std::to_string(x) + ", " + std::to_string(y) + ")",
                   gameTime_, sf::Color(100, 255, 100), 0, teamId);
    addEvent(event);
}

void EventLog::addTeamWin(const std::string& teamName) 
{
    GameEvent event(EventType::TEAM_WIN,
                   "Team " + teamName + " wins the game!",
                   gameTime_, sf::Color(255, 215, 0), 0, 0);
    addEvent(event);
}

void EventLog::addSystemInfo(const std::string& message) 
{
    GameEvent event(EventType::SYSTEM_INFO,
                   message,
                   gameTime_, sf::Color(200, 200, 200), 0, 0);
    addEvent(event);
}

void EventLog::addError(const std::string& message) 
{
    GameEvent event(EventType::ERROR,
                   "ERROR: " + message,
                   gameTime_, sf::Color(255, 50, 50), 0, 0);
    addEvent(event);
}

void EventLog::scrollUp() 
{
    if (scrollPosition_ > 0) {
        scrollPosition_--;
        autoScroll_ = false;
    }
}

void EventLog::scrollDown() 
{
    size_t maxScroll = events_.size() > visibleLines_ ? events_.size() - visibleLines_ : 0;
    if (scrollPosition_ < maxScroll) {
        scrollPosition_++;
        autoScroll_ = false;
    }
}

void EventLog::scrollToTop() 
{
    scrollPosition_ = 0;
    autoScroll_ = false;
}

void EventLog::scrollToBottom() 
{
    size_t maxScroll = events_.size() > visibleLines_ ? events_.size() - visibleLines_ : 0;
    scrollPosition_ = maxScroll;
    autoScroll_ = true;
}

void EventLog::updateScrollPosition() 
{
    visibleLines_ = static_cast<size_t>((area_.height - 2 * padding_ - 20.0f) / lineHeight_);
}

void EventLog::cleanupOldEvents() 
{
    while (events_.size() > maxEvents_) {
        events_.pop_front();
    }
}

std::string EventLog::formatTimestamp(float timestamp) const 
{
    int minutes = static_cast<int>(timestamp) / 60;
    int seconds = static_cast<int>(timestamp) % 60;
    int milliseconds = static_cast<int>((timestamp - static_cast<int>(timestamp)) * 1000);
    return std::to_string(minutes) + ":" +
           (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
           (milliseconds < 100 ? "0" : "") + (milliseconds < 10 ? "0" : "") + std::to_string(milliseconds);
}

}
