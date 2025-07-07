#include "model/Player.hpp"
#include "utils/Math.hpp"
#include "utils/Logger.hpp"

namespace zappy {

Player::Player(PlayerID id, const std::string& teamName, int x, int y, PlayerOrientation orientation)
    : Entity(id)
    , teamName_(teamName)
    , teamId_(0)
    , level_(1)
    , serverPos_(static_cast<float>(x * 32), static_cast<float>(y * 32))
    , interpolatedPos_(static_cast<float>(x * 32), static_cast<float>(y * 32))
    , orientation_(orientation)
    , inventory_{}
    , alive_(true)
    , connected_(true)
    , inIncantation_(false)
    , broadcasting_(false)
    , broadcastMessage_("")
    , animationTime_(0.0f)
    , currentFrame_(0)
{
    LOG_DEBUGF("Player %d created at tile (%d, %d) = pixel (%.1f, %.1f) for team %s", id, x, y, serverPos_.x, serverPos_.y, teamName.c_str());
}

sf::Vector2f Player::getPosition() const
{
    return interpolatedPos_;
}

void Player::setPosition(const sf::Vector2f& position)
{
    setPosition(static_cast<int>(position.x), static_cast<int>(position.y));
}

void Player::setPosition(int x, int y)
{
    setServerPosition(x, y);
}

void Player::setServerPosition(int x, int y)
{
    serverPos_.x = static_cast<float>(x * 32);
    serverPos_.y = static_cast<float>(y * 32);
    if (interpolatedPos_.x == 0.0f && interpolatedPos_.y == 0.0f) {
        interpolatedPos_ = serverPos_;
    }
    LOG_DEBUGF("Player %d server position set to tile (%d, %d) = pixel (%.1f, %.1f)", getId(), x, y, serverPos_.x, serverPos_.y);
}

sf::Vector2f Player::getInterpolatedPosition() const
{
    return interpolatedPos_;
}

void Player::setOrientation(PlayerOrientation orientation)
{
    orientation_ = orientation;
}

PlayerOrientation Player::getOrientation() const
{
    return orientation_;
}

void Player::setLevel(int level)
{
    if (level != level_) {
        LOG_DEBUGF("Player %d level changed: %d -> %d", getId(), level_, level);
        level_ = level;
    }
}

int Player::getLevel() const
{
    return level_;
}

void Player::setTeamName(const std::string& teamName)
{
    if (teamName != teamName_) {
        LOG_DEBUGF("Player %d team changed: %s -> %s", getId(), teamName_.c_str(), teamName.c_str());
        teamName_ = teamName;
    }
}

const std::string& Player::getTeamName() const
{
    return teamName_;
}

TeamID Player::getTeamId() const
{
    return teamId_;
}

void Player::setTeamId(TeamID teamId)
{
    teamId_ = teamId;
}

void Player::setInventory(const std::array<int, 7>& inventory)
{
    inventory_ = inventory;
}

const std::array<int, 7>& Player::getInventory() const
{
    return inventory_;
}

int Player::getResource(ResourceType type) const
{
    int index = static_cast<int>(type);
    if (index >= 0 && index < 7) {
        return inventory_[index];
    }
    return 0;
}

void Player::setResource(ResourceType type, int quantity)
{
    int index = static_cast<int>(type);
    if (index >= 0 && index < 7) {
        inventory_[index] = quantity;
    }
}

void Player::startIncantation()
{
    if (!inIncantation_) {
        LOG_DEBUGF("Player %d started incantation", getId());
        inIncantation_ = true;
    }
}

void Player::stopIncantation()
{
    if (inIncantation_) {
        LOG_DEBUGF("Player %d stopped incantation", getId());
        inIncantation_ = false;
    }
}

bool Player::isInIncantation() const
{
    return inIncantation_;
}

void Player::startBroadcast(const std::string& message)
{
    LOG_DEBUGF("Player %d broadcasting: %s", getId(), message.c_str());
    broadcasting_ = true;
    broadcastMessage_ = message;
}

void Player::stopBroadcast()
{
    if (broadcasting_) {
        LOG_DEBUGF("Player %d stopped broadcasting", getId());
        broadcasting_ = false;
        broadcastMessage_.clear();
    }
}

bool Player::isBroadcasting() const
{
    return broadcasting_;
}

const std::string& Player::getBroadcastMessage() const
{
    return broadcastMessage_;
}

void Player::setAlive(bool alive)
{
    if (alive != alive_) {
        LOG_DEBUGF("Player %d %s", getId(), alive ? "revived" : "died");
        alive_ = alive;
    }
}

bool Player::isAlive() const
{
    return alive_;
}

void Player::setConnected(bool connected)
{
    if (connected != connected_) {
        LOG_DEBUGF("Player %d %s", getId(), connected ? "connected" : "disconnected");
        connected_ = connected;
    }
}

bool Player::isConnected() const
{
    return connected_;
}

void Player::update(float deltaTime)
{
    Entity::update(deltaTime);
    updateInterpolation(deltaTime);
    updateAnimations(deltaTime);
}

sf::Color Player::getTeamColor() const
{
    std::hash<std::string> hasher;
    size_t hash = hasher(teamName_);
    float hue = (hash % 360) / 360.0f;
    return math::hsvToRgb(hue, 0.8f, 0.9f);
}

sf::FloatRect Player::getBounds() const
{
    const float size = 24.0f;
    return sf::FloatRect(
        interpolatedPos_.x - size / 2,
        interpolatedPos_.y - size / 2,
        size, size
    );
}

void Player::updateInterpolation(float deltaTime) {
    if (serverPos_ != interpolatedPos_) {
        float speed = 5.0f;
        interpolatedPos_ = math::lerp(interpolatedPos_, serverPos_, deltaTime * speed);
        if (math::distance(interpolatedPos_, serverPos_) < 0.1f) {
            interpolatedPos_ = serverPos_;
        }
    }
}


void Player::updateAnimations(float deltaTime) {
    animationTime_ += deltaTime;
    const float frameTime = 0.2f;
    if (animationTime_ >= frameTime) {
        animationTime_ -= frameTime;
        currentFrame_ = (currentFrame_ + 1) % 4;
    }
}


int Player::getCurrentFrame() const {
    return currentFrame_;
}

}
