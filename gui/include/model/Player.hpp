#pragma once
#include "Common.hpp"
#include "model/Entity.hpp"
namespace zappy {
enum class PlayerOrientation {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4
};
class Player : public Entity {
public:
    Player(PlayerID id, const std::string& teamName, int x, int y, PlayerOrientation orientation);
    ~Player() = default;

    sf::Vector2f getPosition() const override;
    void setPosition(const sf::Vector2f& position) override;
    void setPosition(int x, int y);
    void setServerPosition(int x, int y);
    sf::Vector2f getInterpolatedPosition() const;

    void setOrientation(PlayerOrientation orientation);
    PlayerOrientation getOrientation() const;

    void setLevel(int level);
    int getLevel() const;

    void setTeamName(const std::string& teamName);
    const std::string& getTeamName() const;

    TeamID getTeamId() const;
    void setTeamId(TeamID teamId);

    void setInventory(const std::array<int, 7>& inventory);
    const std::array<int, 7>& getInventory() const;
    int getResource(ResourceType type) const;
    void setResource(ResourceType type, int quantity);

    void startIncantation();
    void stopIncantation();
    bool isInIncantation() const;

    void startBroadcast(const std::string& message);
    void stopBroadcast();
    bool isBroadcasting() const;
    const std::string& getBroadcastMessage() const;

    void setAlive(bool alive);
    bool isAlive() const;

    void setConnected(bool connected);
    bool isConnected() const;

    void update(float deltaTime) override;

    sf::Color getTeamColor() const;
    sf::FloatRect getBounds() const override;

    int getCurrentFrame() const;

private:
    void updateInterpolation(float deltaTime);
    void updateAnimations(float deltaTime);

    std::string teamName_;
    TeamID teamId_;
    int level_;
    sf::Vector2f serverPos_;
    sf::Vector2f interpolatedPos_;
    PlayerOrientation orientation_;
    std::array<int, 7> inventory_;
    bool alive_;
    bool connected_;
    bool inIncantation_;
    bool broadcasting_;
    std::string broadcastMessage_;
    float animationTime_;
    int currentFrame_;
};
}
