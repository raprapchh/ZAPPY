#include "model/Team.hpp"
#include "utils/Logger.hpp"
#include <algorithm>

namespace zappy {

Team::Team(TeamID id, const std::string& name)
    : id_(id)
    , name_(name)
    , color_(sf::Color::White)
    , levelCounts_({0})
    , totalResources_({0})
    , victoryProgress_(0.0f)
    , deaths_(0)
    , elevations_(0)
    , maxSlots_(0)
{
    LOG_DEBUGF("Team %s (ID: %d) created", name_.c_str(), id_);
}

TeamID Team::getId() const
{
    return id_;
}

const std::string& Team::getName() const
{
    return name_;
}

void Team::setColor(const sf::Color& color)
{
    color_ = color;
}

sf::Color Team::getColor() const
{
    return color_;
}

void Team::addPlayer(PlayerID playerId)
{
    players_.push_back(playerId);
    LOG_DEBUGF("Player %d added to team %s", playerId, name_.c_str());
}

void Team::removePlayer(PlayerID playerId)
{
    players_.erase(std::remove(players_.begin(), players_.end(), playerId), players_.end());
    LOG_DEBUGF("Player %d removed from team %s", playerId, name_.c_str());
}

const std::vector<PlayerID>& Team::getPlayers() const
{
    return players_;
}

size_t Team::getPlayerCount() const
{
    return players_.size();
}

void Team::updatePlayerLevel(PlayerID playerId, int level)
{
    if (level >= 1 && level <= 8) {
        levelCounts_[level - 1]++;
    }
    LOG_DEBUGF("Player %d level updated to %d for team %s", playerId, name_.c_str());
}

std::array<int, 8> Team::getLevelDistribution() const
{
    return levelCounts_;
}

int Team::getPlayersAtLevel(int level) const
{
    if (level >= 1 && level <= 8) {
        return levelCounts_[level - 1];
    }
    return 0;
}

int Team::getMaxLevel() const
{
    for (int i = 7; i >= 0; --i) {
        if (levelCounts_[i] > 0) {
            return i + 1;
        }
    }
    return 0;
}

void Team::updateTeamResources(const std::array<int, 7>& resources)
{
    totalResources_ = resources;
    LOG_DEBUGF("Team %s resources updated", name_.c_str());
}

const std::array<int, 7>& Team::getTotalResources() const
{
    return totalResources_;
}

int Team::getTotalResource(ResourceType type) const
{
    if (static_cast<size_t>(type) < totalResources_.size()) {
        return totalResources_[static_cast<int>(type)];
    }
    return 0;
}


void Team::addEgg(EggID eggId)
{
    eggs_.push_back(eggId);
    LOG_DEBUGF("Egg %d added to team %s", eggId, name_.c_str());
}

void Team::removeEgg(EggID eggId)
{
    eggs_.erase(std::remove(eggs_.begin(), eggs_.end(), eggId), eggs_.end());
    LOG_DEBUGF("Egg %d removed from team %s", eggId, name_.c_str());
}

const std::vector<EggID>& Team::getEggs() const
{
    return eggs_;
}

size_t Team::getEggCount() const
{
    return eggs_.size();
}

void Team::updateVictoryProgress()
{
    int playersAtMaxLevel = getPlayersAtLevel(8);

    if (maxSlots_ > 0) {
        victoryProgress_ = static_cast<float>(playersAtMaxLevel) / maxSlots_;
    } else {
        victoryProgress_ = 0.0f;
    }
}

bool Team::hasWon() const
{
    return getPlayersAtLevel(8) >= 6;
}

void Team::incrementDeaths()
{
    deaths_++;
}

void Team::incrementElevations() 
{
    elevations_++;
}

int Team::getDeaths() const 
{
    return deaths_;
}

int Team::getElevations() const 
{
    return elevations_;
}

void Team::setMaxSlots(int slots) 
{
    maxSlots_ = slots;
}

int Team::getMaxSlots() const 
{
    return maxSlots_;
}

int Team::getAvailableSlots() const 
{
    return maxSlots_ - static_cast<int>(players_.size());
}

}
