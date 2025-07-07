#pragma once
#include "Common.hpp"
#include "events/EventDispatcher.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Team.hpp"
#include "Egg.hpp"
#include "Tile.hpp"
namespace zappy {
class World {
public:
    explicit World(EventDispatcher& dispatcher);
    ~World() = default;

    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) = default;
    World& operator=(World&&) = default;

    void setMapSize(int width, int height);
    sf::Vector2i getMapSize() const;

    const Tile& getTile(int x, int y) const;
    Tile& getTile(int x, int y);
    bool isValidPosition(int x, int y) const;

    void addPlayer(std::unique_ptr<Player> player);
    void removePlayer(PlayerID id);
    Player* getPlayer(PlayerID id);
    const Player* getPlayer(PlayerID id) const;

    void addTeam(std::unique_ptr<Team> team);
    Team* getTeam(TeamID id);
    const Team* getTeam(TeamID id) const;
    Team* getTeamByName(const std::string& name);

    void addEgg(std::unique_ptr<Egg> egg);
    void removeEgg(EggID id);
    Egg* getEgg(EggID id);

    const std::unordered_map<PlayerID, std::unique_ptr<Player>>& getPlayers() const;
    const std::unordered_map<TeamID, std::unique_ptr<Team>>& getTeams() const;
    const std::unordered_map<EggID, std::unique_ptr<Egg>>& getEggs() const;

    std::vector<Player*> getPlayersAt(int x, int y);
    std::vector<Egg*> getEggsAt(int x, int y);
    std::vector<Entity*> getVisibleEntities(const sf::FloatRect& bounds);

    void setTimeUnit(int timeUnit);
    int getTimeUnit() const;

    void setGameEnded(bool ended, const std::string& winnerTeam = "");
    bool isGameEnded() const;
    const std::string& getWinnerTeam() const;

    void update(float deltaTime);

    size_t getPlayerCount() const;
    size_t getTeamCount() const;
    size_t getEggCount() const;

private:
    void initializeTiles();
    void normalizeCoordinates(int& x, int& y) const;

    EventDispatcher& dispatcher_;
    int mapWidth_{0};
    int mapHeight_{0};
    std::vector<std::vector<Tile>> tiles_;
    std::unordered_map<PlayerID, std::unique_ptr<Player>> players_;
    std::unordered_map<TeamID, std::unique_ptr<Team>> teams_;
    std::unordered_map<EggID, std::unique_ptr<Egg>> eggs_;
    int timeUnit_{100};
    bool gameEnded_{false};
    std::string winnerTeam_;
    mutable std::vector<Entity*> visibilityCache_;
    mutable sf::FloatRect lastQueryBounds_;
    mutable bool visibilityCacheValid_{false};
};
}
