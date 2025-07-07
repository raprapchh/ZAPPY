#pragma once
#include "Common.hpp"
namespace zappy {
class Player;
class Egg;
class Tile {
public:
    Tile(int x, int y);
    ~Tile() = default;

    int getX() const;
    int getY() const;
    sf::Vector2i getPosition() const;

    void setResources(const std::array<int, 7>& resources);
    const std::array<int, 7>& getResources() const;
    int getResource(ResourceType type) const;
    void setResource(ResourceType type, int quantity);
    void addResource(ResourceType type, int quantity = 1);
    void removeResource(ResourceType type, int quantity = 1);
    int getTotalResources() const;
    bool hasResources() const;
    bool hasResource(ResourceType type) const;
    std::vector<ResourceType> getAvailableResources() const;

    void addPlayer(PlayerID playerId);
    void removePlayer(PlayerID playerId);
    const std::vector<PlayerID>& getPlayers() const;
    bool hasPlayers() const;
    size_t getPlayerCount() const;

    void addEgg(EggID eggId);
    void removeEgg(EggID eggId);
    const std::vector<EggID>& getEggs() const;
    bool hasEggs() const;
    size_t getEggCount() const;

    void setIncantationActive(bool active);
    bool hasIncantation() const;
    void setIncantationLevel(int level);
    int getIncantationLevel() const;

    sf::Color getTileColor() const;
    float getResourceDensity() const;
    bool shouldHighlight() const;

    void update(float deltaTime);

private:
    int x_, y_;
    std::array<int, 7> resources_;
    std::vector<PlayerID> players_;
    std::vector<EggID> eggs_;
    bool incantationActive_;
    int incantationLevel_;
    float animationTime_;
};
}
