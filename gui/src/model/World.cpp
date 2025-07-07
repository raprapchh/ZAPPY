#include "model/World.hpp"
#include "events/Event.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
namespace zappy {
World::World(EventDispatcher& dispatcher)
    : dispatcher_(dispatcher) 
{
    LOG_DEBUG("World created");
}
void World::setMapSize(int width, int height) 
{
    mapWidth_ = width;
    mapHeight_ = height;
    initializeTiles();
    LOG_DEBUGF("World map size set to %dx%d", width, height);
}

sf::Vector2i World::getMapSize() const 
{
    return {mapWidth_, mapHeight_};
}

const Tile& World::getTile(int x, int y) const 
{
    normalizeCoordinates(x, y);
    return tiles_[static_cast<size_t>(y)][static_cast<size_t>(x)];
}

Tile& World::getTile(int x, int y)
{
    normalizeCoordinates(x, y);
    return tiles_[static_cast<size_t>(y)][static_cast<size_t>(x)];
}

bool World::isValidPosition(int x, int y) const 
{
    return x >= 0 && x < mapWidth_ && y >= 0 && y < mapHeight_;
}

void World::addPlayer(std::unique_ptr<Player> player) 
{
    if (!player) return;
    PlayerID id = player->getId();
    const auto* p = player.get();
    sf::Vector2f pos = p->getPosition();
    sf::Vector2i tilePos(static_cast<int>(pos.x / 32.0f), static_cast<int>(pos.y / 32.0f));
    players_[id] = std::move(player);
    dispatcher_.post(PlayerAddedEvent{id, tilePos, static_cast<int>(p->getOrientation()), p->getLevel(), p->getTeamName()});
    visibilityCacheValid_ = false;
    LOG_DEBUGF("Player %d added to world", id);
}

void World::removePlayer(PlayerID id) 
{
    auto it = players_.find(id);
    if (it != players_.end()) {
        dispatcher_.post(PlayerRemovedEvent{id});
        players_.erase(it);
        visibilityCacheValid_ = false;
        LOG_DEBUGF("Player %d removed from world", id);
    }
}

Player* World::getPlayer(PlayerID id) 
{
    auto it = players_.find(id);
    return (it != players_.end()) ? it->second.get() : nullptr;
}

const Player* World::getPlayer(PlayerID id) const 
{
    auto it = players_.find(id);
    return (it != players_.end()) ? it->second.get() : nullptr;
}

void World::addTeam(std::unique_ptr<Team> team) 
{
    if (!team) return;
    TeamID id = team->getId();
    teams_[id] = std::move(team);
    LOG_DEBUGF("Team %d added to world", id);
}

Team* World::getTeam(TeamID id) 
{
    auto it = teams_.find(id);
    return (it != teams_.end()) ? it->second.get() : nullptr;
}

const Team* World::getTeam(TeamID id) const 
{
    auto it = teams_.find(id);
    return (it != teams_.end()) ? it->second.get() : nullptr;
}

Team* World::getTeamByName(const std::string& name) 
{
    for (auto& [id, team] : teams_) {
        if (team->getName() == name) {
            return team.get();
        }
    }
    return nullptr;
}

void World::addEgg(std::unique_ptr<Egg> egg) 
{
    if (!egg) return;
    EggID id = egg->getId();
    const auto* e = egg.get();
    sf::Vector2f pos = e->getPosition();
    sf::Vector2i tilePos(static_cast<int>(pos.x / 32.0f), static_cast<int>(pos.y / 32.0f));
    eggs_[id] = std::move(egg);
    dispatcher_.post(EggAddedEvent{id, e->getConnectedPlayer(), tilePos});
    visibilityCacheValid_ = false;
    LOG_DEBUGF("Egg %d added to world", id);
}

void World::removeEgg(EggID id) 
{
    auto it = eggs_.find(id);
    if (it != eggs_.end()) {
        dispatcher_.post(EggRemovedEvent{id});
        eggs_.erase(it);
        visibilityCacheValid_ = false;
        LOG_DEBUGF("Egg %d removed from world", id);
    }
}

Egg* World::getEgg(EggID id) 
{
    auto it = eggs_.find(id);
    return (it != eggs_.end()) ? it->second.get() : nullptr;
}

const std::unordered_map<PlayerID, std::unique_ptr<Player>>& World::getPlayers() const {
    return players_;
}


const std::unordered_map<TeamID, std::unique_ptr<Team>>& World::getTeams() const {
    return teams_;
}


const std::unordered_map<EggID, std::unique_ptr<Egg>>& World::getEggs() const {
    return eggs_;
}

void World::setTimeUnit(int timeUnit) 
{
    timeUnit_ = timeUnit;
}

int World::getTimeUnit() const 
{
    return timeUnit_;
}

void World::setGameEnded(bool ended, const std::string& winnerTeam) 
{
    gameEnded_ = ended;
    winnerTeam_ = winnerTeam;
    if (ended) {
        dispatcher_.post(GameEndedEvent{winnerTeam});
    }
}

bool World::isGameEnded() const 
{
    return gameEnded_;
}

const std::string& World::getWinnerTeam() const 
{
    return winnerTeam_;
}

std::vector<Player*> World::getPlayersAt(int x, int y) 
{
    std::vector<Player*> result;
    for (auto& [id, player] : players_) {
        sf::Vector2f pos = player->getPosition();
        int playerX = static_cast<int>(pos.x / 32.0f);
        int playerY = static_cast<int>(pos.y / 32.0f);
        if (playerX == x && playerY == y) {
            result.push_back(player.get());
        }
    }
    return result;
}

std::vector<Egg*> World::getEggsAt(int x, int y) 
{
    std::vector<Egg*> result;

    for (auto& [id, egg] : eggs_) {
        sf::Vector2f pos = egg->getPosition();
        int eggX = static_cast<int>(pos.x / 32.0f);
        int eggY = static_cast<int>(pos.y / 32.0f);
        if (eggX == x && eggY == y) {
            result.push_back(egg.get());
        }
    }
    return result;
}

std::vector<Entity*> World::getVisibleEntities(const sf::FloatRect& bounds) 
{
    if (visibilityCacheValid_ && lastQueryBounds_ == bounds) {
        return visibilityCache_;
    }
    visibilityCache_.clear();
    for (auto& [id, player] : players_) {
        sf::Vector2f pos = player->getPosition();
        if (bounds.contains(pos)) {
            visibilityCache_.push_back(player.get());
        }
    }
    for (auto& [id, egg] : eggs_) {
        sf::Vector2f pos = egg->getPosition();
        if (bounds.contains(pos)) {
            visibilityCache_.push_back(egg.get());
        }
    }
    lastQueryBounds_ = bounds;
    visibilityCacheValid_ = true;
    return visibilityCache_;
}

void World::update(float deltaTime) 
{
    for (auto& [id, player] : players_) {
        player->update(deltaTime);
    }
    for (auto& [id, egg] : eggs_) {
        egg->update(deltaTime);
    }
    visibilityCacheValid_ = false;
}

size_t World::getPlayerCount() const 
{
    return players_.size();
}

size_t World::getTeamCount() const 
{
    return teams_.size();
}

size_t World::getEggCount() const 
{
    return eggs_.size();
}

void World::initializeTiles() 
{
    tiles_.clear();
    tiles_.resize(static_cast<size_t>(mapHeight_));
    for (int y = 0; y < mapHeight_; ++y) {
        tiles_[static_cast<size_t>(y)].clear();
        for (int x = 0; x < mapWidth_; ++x) {
            tiles_[static_cast<size_t>(y)].push_back(Tile(x, y));
        }
    }
    LOG_DEBUGF("Initialized %dx%d tile grid", mapWidth_, mapHeight_);
}

void World::normalizeCoordinates(int& x, int& y) const 
{
    if (mapWidth_ > 0 && mapHeight_ > 0) {
        x = ((x % mapWidth_) + mapWidth_) % mapWidth_;
        y = ((y % mapHeight_) + mapHeight_) % mapHeight_;
    }
}

}
