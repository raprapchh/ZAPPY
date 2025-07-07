#pragma once
#include "Common.hpp"
namespace zappy {
class Team {
public:
    Team(TeamID id, const std::string& name);
    ~Team() = default;

    TeamID getId() const;
    const std::string& getName() const;

    void setColor(const sf::Color& color);
    sf::Color getColor() const;

    void addPlayer(PlayerID playerId);
    void removePlayer(PlayerID playerId);
    const std::vector<PlayerID>& getPlayers() const;
    size_t getPlayerCount() const;

    void updatePlayerLevel(PlayerID playerId, int level);
    std::array<int, 8> getLevelDistribution() const;
    int getPlayersAtLevel(int level) const;
    int getMaxLevel() const;

    void updateTeamResources(const std::array<int, 7>& resources);
    const std::array<int, 7>& getTotalResources() const;
    int getTotalResource(ResourceType type) const;

    void addEgg(EggID eggId);
    void removeEgg(EggID eggId);
    const std::vector<EggID>& getEggs() const;
    size_t getEggCount() const;

    void updateVictoryProgress();
    bool hasWon() const;

    void incrementDeaths();
    void incrementElevations();
    int getDeaths() const;
    int getElevations() const;

    void setMaxSlots(int slots);
    int getMaxSlots() const;
    int getAvailableSlots() const;

private:
    TeamID id_;
    std::string name_;
    sf::Color color_;
    std::vector<PlayerID> players_;
    std::array<int, 8> levelCounts_;
    std::array<int, 7> totalResources_;
    std::vector<EggID> eggs_;
    float victoryProgress_;
    int deaths_;
    int elevations_;
    int maxSlots_;
};
}
