#pragma once
#include "Common.hpp"
#include <string>
namespace zappy {
struct Event {
    virtual ~Event() = default;
};
struct PlayerAddedEvent : public Event {
    PlayerID id;
    sf::Vector2i position;
    int orientation;
    int level;
    std::string teamName;
    PlayerAddedEvent(PlayerID id, sf::Vector2i pos, int orient, int lvl, std::string team)
        : id(id), position(pos), orientation(orient), level(lvl), teamName(std::move(team)) {}
};
struct PlayerRemovedEvent : public Event {
    PlayerID id;
    explicit PlayerRemovedEvent(PlayerID id) : id(id) {}
};
struct PlayerMovedEvent : public Event {
    PlayerID id;
    sf::Vector2i newPosition;
    int newOrientation;
    PlayerMovedEvent(PlayerID id, sf::Vector2i pos, int orient)
        : id(id), newPosition(pos), newOrientation(orient) {}
};
struct PlayerLevelUpEvent : public Event {
    PlayerID id;
    int newLevel;
    PlayerLevelUpEvent(PlayerID id, int level) : id(id), newLevel(level) {}
};
struct PlayerIncantationEvent : public Event {
    PlayerID id;
    bool isStarting;
    PlayerIncantationEvent(PlayerID id, bool starting) : id(id), isStarting(starting) {}
};
struct TileChangedEvent : public Event {
    sf::Vector2i position;
    explicit TileChangedEvent(sf::Vector2i pos) : position(pos) {}
};
struct EggAddedEvent : public Event {
    EggID id;
    PlayerID parentPlayerId;
    sf::Vector2i position;
    EggAddedEvent(EggID id, PlayerID parentId, sf::Vector2i pos)
        : id(id), parentPlayerId(parentId), position(pos) {}
};
struct EggRemovedEvent : public Event {
    EggID id;
    explicit EggRemovedEvent(EggID id) : id(id) {}
};
struct GameEndedEvent : public Event {
    std::string winnerTeamName;
    explicit GameEndedEvent(std::string winner) : winnerTeamName(std::move(winner)) {}
};
}
