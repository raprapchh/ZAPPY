#pragma once
#include "Common.hpp"
#include "model/Entity.hpp"
namespace zappy {
enum class EggState {
    LAYING,
    INCUBATING,
    HATCHING
};
class Egg : public Entity {
public:
    Egg(EggID id, TeamID teamId, int x, int y);
    ~Egg() = default;
    TeamID getTeamId() const { return teamId_; }
    void setTeamId(TeamID teamId) { teamId_ = teamId; }
    EggState getState() const { return state_; }
    void setIncubationTime(float time) { incubationTime_ = time; }
    float getIncubationTime() const { return incubationTime_; }
    PlayerID getConnectedPlayer() const { return connectedPlayerId_; }
    bool hasConnectedPlayer() const { return connectedPlayerId_ != 0; }
    sf::Color getTeamColor() const;
    sf::FloatRect getBounds() const override;
    float getPulseIntensity() const { return pulseIntensity_; }
private:
    TeamID teamId_;
    EggState state_;
    float incubationTime_;
    float maxIncubationTime_;
    PlayerID connectedPlayerId_;
    float animationTime_;
    float pulseIntensity_;
};
}
