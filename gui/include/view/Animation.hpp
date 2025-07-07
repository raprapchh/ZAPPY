#pragma once
#include "Common.hpp"
#include "model/Player.hpp"
namespace zappy {
struct AnimationFrame {
    sf::IntRect rect;
    float duration;
};
class Animation {
public:
    Animation() = default;
    Animation(const std::vector<AnimationFrame>& frames, bool loop = true);
    ~Animation() = default;

    void addFrame(const sf::IntRect& rect, float duration);
    void setLoop(bool loop);
    void setSpeed(float speed);
    void update(float deltaTime);
    void reset();
    void pause();
    void play();
    sf::IntRect getCurrentFrame() const;
    bool isFinished() const;
    bool isPlaying() const;

private:
    std::vector<AnimationFrame> frames_;
    size_t currentFrame_;
    float currentTime_;
    float speed_;
    bool loop_;
    bool playing_;
};

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem() = default;

    bool initialize();
    void update(float deltaTime);
    void registerAnimation(const std::string& name, const Animation& animation);
    Animation* getAnimation(const std::string& name);
    Animation* getPlayerWalkAnimation(PlayerOrientation orientation);
    Animation* getPlayerIdleAnimation(PlayerOrientation orientation);
    Animation* getPlayerIncantationAnimation();
    Animation* getResourceSparkleAnimation();
    Animation* getBroadcastWaveAnimation();
    Animation* getEggPulseAnimation();
    void applyAnimation(sf::Sprite& sprite, const std::string& animationName, float deltaTime);
    void applyPlayerAnimation(sf::Sprite& sprite, PlayerOrientation orientation,
                             bool moving, float deltaTime);

private:
    void loadPlayerAnimations();
    void loadEffectAnimations();
    void createWalkAnimation(PlayerOrientation orientation);
    void createIdleAnimation(PlayerOrientation orientation);

    std::map<std::string, Animation> animations_;
    std::map<PlayerOrientation, std::vector<sf::IntRect>> walkFrames_;
    std::map<PlayerOrientation, std::vector<sf::IntRect>> idleFrames_;
    float frameTime_;
    float effectFrameTime_;
};
}
