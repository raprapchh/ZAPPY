#include "model/Tile.hpp"
#include "utils/Logger.hpp"
#include <set>
#include <algorithm>
#include <array>
#include <chrono>
#include <vector>
#include <SFML/Graphics.hpp>

namespace zappy {

Tile::Tile(int x, int y)
    : x_(x)
    , y_(y)
    , resources_{}
    , players_{}
    , eggs_{}
    , incantationActive_(false)
    , incantationLevel_(0)
    , animationTime_(0.0f)
{
    LOG_DEBUGF("Tile created at (%d, %d)", x, y);
}

int Tile::getX() const 
{
    return x_;
}

int Tile::getY() const
{
    return y_;
}

sf::Vector2i Tile::getPosition() const 
{
    return {x_, y_};
}

void Tile::setResource(ResourceType type, int quantity) 
{
    if (static_cast<int>(type) >= 0 && static_cast<int>(type) < 7) {
        resources_[static_cast<int>(type)] = std::max(0, quantity);
        LOG_DEBUGF("Tile (%d, %d) resource %d set to %d", x_, y_, static_cast<int>(type), quantity);
    }
}

int Tile::getResource(ResourceType type) const 
{
    int index = static_cast<int>(type);
    if (index >= 0 && index < 7) {
        return resources_[index];
    }
    return 0;
}

void Tile::addResource(ResourceType type, int quantity)
{
    if (static_cast<int>(type) >= 0 && static_cast<int>(type) < 7 && quantity > 0) {
        resources_[static_cast<int>(type)] += quantity;
        LOG_DEBUGF("Tile (%d, %d) resource %d increased by %d", x_, y_, static_cast<int>(type), quantity);
    }
}

void Tile::removeResource(ResourceType type, int quantity) 
{
    if (static_cast<int>(type) >= 0 && static_cast<int>(type) < 7 && quantity > 0) {
        resources_[static_cast<int>(type)] = std::max(0, resources_[static_cast<int>(type)] - quantity);
        LOG_DEBUGF("Tile (%d, %d) resource %d decreased by %d", x_, y_, static_cast<int>(type), quantity);
    }
}

void Tile::setResources(const std::array<int, 7>& resources) 
{
    resources_ = resources;
    LOG_DEBUGF("Tile (%d, %d) resources updated", x_, y_);
}

const std::array<int, 7>& Tile::getResources() const {
    return resources_;
}


int Tile::getTotalResources() const 
{
    int total = 0;
    for (int resource : resources_) {
        total += resource;
    }
    return total;
}

bool Tile::hasResources() const 
{
    return getTotalResources() > 0;
}

bool Tile::hasResource(ResourceType type) const 
{
    if (static_cast<int>(type) >= 0 && static_cast<int>(type) < 7) {
        return resources_[static_cast<int>(type)] > 0;
    }
    return false;
}

std::vector<ResourceType> Tile::getAvailableResources() const 
{
    std::vector<ResourceType> available;
    for (int i = 0; i < 7; ++i) {
        if (resources_[i] > 0) {
            available.push_back(static_cast<ResourceType>(i));
        }
    }
    return available;
}

void Tile::addPlayer(PlayerID id) 
{
    if (std::find(players_.begin(), players_.end(), id) == players_.end()) {
        players_.push_back(id);
        LOG_DEBUGF("Player %d added to tile (%d, %d)", id, x_, y_);
    }
}

void Tile::removePlayer(PlayerID id) 
{
    auto it = std::find(players_.begin(), players_.end(), id);
    if (it != players_.end()) {
        players_.erase(it);
        LOG_DEBUGF("Player %d removed from tile (%d, %d)", id, x_, y_);
    }
}

const std::vector<PlayerID>& Tile::getPlayers() const 
{
    return players_;
}

bool Tile::hasPlayers() const 
{
    return !players_.empty();
}

size_t Tile::getPlayerCount() const 
{
    return players_.size();
}

void Tile::addEgg(EggID id) 
{
    if (std::find(eggs_.begin(), eggs_.end(), id) == eggs_.end()) {
        eggs_.push_back(id);
        LOG_DEBUGF("Egg %d added to tile (%d, %d)", id, x_, y_);
    }
}

void Tile::removeEgg(EggID id) 
{
    auto it = std::find(eggs_.begin(), eggs_.end(), id);
    if (it != eggs_.end()) {
        eggs_.erase(it);
        LOG_DEBUGF("Egg %d removed from tile (%d, %d)", id, x_, y_);
    }
}

const std::vector<EggID>& Tile::getEggs() const 
{
    return eggs_;
}

bool Tile::hasEggs() const 
{
    return !eggs_.empty();
}

size_t Tile::getEggCount() const 
{
    return eggs_.size();
}

void Tile::setIncantationActive(bool active) 
{
    incantationActive_ = active;
    if (!active) {
        incantationLevel_ = 0;
    }
}

bool Tile::hasIncantation() const 
{
    return incantationActive_;
}

void Tile::setIncantationLevel(int level) 
{
    incantationLevel_ = level;
}

int Tile::getIncantationLevel() const 
{
    return incantationLevel_;
}

sf::Color Tile::getTileColor() const 
{
    sf::Color baseColor(34, 139, 34);
    if (hasResources()) {
        float density = getResourceDensity();
        int brightness = static_cast<int>(density * 50);
        baseColor.r = std::min(255, baseColor.r + brightness);
        baseColor.g = std::min(255, baseColor.g + brightness);
        baseColor.b = std::min(255, baseColor.b + brightness);
    }
    return baseColor;
}

float Tile::getResourceDensity() const 
{
    const int maxResourcesPerTile = 50;
    return std::min(1.0f, static_cast<float>(getTotalResources()) / maxResourcesPerTile);
}

bool Tile::shouldHighlight() const 
{
    return incantationActive_ || getTotalResources() > 10;
}

void Tile::update(float ) 
{
    animationTime_ += 0.016f;
}

}
