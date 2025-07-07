#include "view/Animation.hpp"
#include "utils/Logger.hpp"

namespace zappy {

Animation::Animation(const std::vector<AnimationFrame>& frames, bool loop)
    : frames_(frames)
    , currentFrame_(0)
    , currentTime_(0.0f)
    , speed_(1.0f)
    , loop_(loop)
    , playing_(true) 
{
}


void Animation::addFrame(const sf::IntRect& rect, float duration) 
{
    frames_.push_back({rect, duration});
}

void Animation::setLoop(bool loop) 
{
    loop_ = loop;
}

void Animation::setSpeed(float speed) 
{
    speed_ = speed;
}

void Animation::update(float deltaTime) 
{
    if (!playing_ || frames_.empty()) return;
    currentTime_ += deltaTime * speed_;
    if (currentFrame_ < frames_.size()) {
        float frameDuration = frames_[currentFrame_].duration;
        if (currentTime_ >= frameDuration) {
            currentTime_ -= frameDuration;
            currentFrame_++;
            if (currentFrame_ >= frames_.size()) {
                if (loop_) {
                    currentFrame_ = 0;
                } else {
                    currentFrame_ = frames_.size() - 1;
                    playing_ = false;
                }
            }
        }
    }
}

void Animation::reset() 
{
    currentFrame_ = 0;
    currentTime_ = 0.0f;
    playing_ = true;
}

void Animation::pause() 
{
    playing_ = false;
}

void Animation::play() 
{
    playing_ = true;
}

sf::IntRect Animation::getCurrentFrame() const 
{
    if (frames_.empty() || currentFrame_ >= frames_.size()) {
        return sf::IntRect(0, 0, 32, 32);
    }
    return frames_[currentFrame_].rect;
}

bool Animation::isFinished() const 
{
    return !loop_ && currentFrame_ >= frames_.size() - 1;
}

bool Animation::isPlaying() const 
{
    return playing_;
}

AnimationSystem::AnimationSystem()
    : frameTime_(0.2f)
    , effectFrameTime_(0.1f) 
{
}


bool AnimationSystem::initialize() 
{
    loadPlayerAnimations();
    loadEffectAnimations();
    LOG_DEBUG("AnimationSystem initialized");
    return true;
}

void AnimationSystem::update(float deltaTime) 
{
    for (auto& [name, animation] : animations_) {
        animation.update(deltaTime);
    }
}

void AnimationSystem::registerAnimation(const std::string& name, const Animation& animation) 
{
    animations_[name] = animation;
}

Animation* AnimationSystem::getAnimation(const std::string& name) 
{
    auto it = animations_.find(name);
    return (it != animations_.end()) ? &it->second : nullptr;
}

Animation* AnimationSystem::getPlayerWalkAnimation(PlayerOrientation orientation) 
{
    std::string animName = "player_walk_";
    switch (orientation) {
        case PlayerOrientation::NORTH:
            animName += "north";
            break;
        case PlayerOrientation::SOUTH:
            animName += "south";
            break;
        case PlayerOrientation::EAST:
            animName += "east";
            break;
        case PlayerOrientation::WEST:
            animName += "west";
            break;
    }
    return getAnimation(animName);
}

Animation* AnimationSystem::getPlayerIdleAnimation(PlayerOrientation orientation) 
{
    std::string animName = "player_idle_";
    switch (orientation) {
        case PlayerOrientation::NORTH:
            animName += "north";
            break;
        case PlayerOrientation::SOUTH:
            animName += "south";
            break;
        case PlayerOrientation::EAST:
            animName += "east";
            break;
        case PlayerOrientation::WEST:
            animName += "west";
            break;
    }
    return getAnimation(animName);
}

Animation* AnimationSystem::getPlayerIncantationAnimation() 
{
    return getAnimation("player_incantation");
}

Animation* AnimationSystem::getResourceSparkleAnimation() 
{
    return getAnimation("resource_sparkle");
}

Animation* AnimationSystem::getBroadcastWaveAnimation() 
{
    return getAnimation("broadcast_wave");
}

Animation* AnimationSystem::getEggPulseAnimation() 
{
    return getAnimation("egg_pulse");
}

void AnimationSystem::applyAnimation(sf::Sprite& sprite, const std::string& animationName, float deltaTime) 
{
    Animation* anim = getAnimation(animationName);
    if (anim) {
        anim->update(deltaTime);
        sprite.setTextureRect(anim->getCurrentFrame());
    }
}

void AnimationSystem::applyPlayerAnimation(sf::Sprite& sprite, PlayerOrientation orientation,
        bool moving, float deltaTime) 
{
    Animation* anim = nullptr;
    if (moving) {
        anim = getPlayerWalkAnimation(orientation);
    } else {
        anim = getPlayerIdleAnimation(orientation);
    }
    if (anim) {
        anim->update(deltaTime);
        sprite.setTextureRect(anim->getCurrentFrame());
    }
}

void AnimationSystem::loadPlayerAnimations() 
{
    createWalkAnimation(PlayerOrientation::NORTH);
    createWalkAnimation(PlayerOrientation::SOUTH);
    createWalkAnimation(PlayerOrientation::EAST);
    createWalkAnimation(PlayerOrientation::WEST);
    createIdleAnimation(PlayerOrientation::NORTH);
    createIdleAnimation(PlayerOrientation::SOUTH);
    createIdleAnimation(PlayerOrientation::EAST);
    createIdleAnimation(PlayerOrientation::WEST);

    Animation incantationAnim;
    for (int i = 0; i < 8; ++i) {
        incantationAnim.addFrame(sf::IntRect(i * 32, 128, 32, 32), effectFrameTime_);
    }
    incantationAnim.setLoop(true);
    registerAnimation("player_incantation", incantationAnim);
}

void AnimationSystem::loadEffectAnimations() 
{
    Animation sparkleAnim;
    for (int i = 0; i < 6; ++i) {
        sparkleAnim.addFrame(sf::IntRect(i * 16, 160, 16, 16), effectFrameTime_);
    }
    sparkleAnim.setLoop(true);
    registerAnimation("resource_sparkle", sparkleAnim);

    Animation waveAnim;
    for (int i = 0; i < 4; ++i) {
        waveAnim.addFrame(sf::IntRect(i * 64, 176, 64, 64), effectFrameTime_ * 2);
    }
    waveAnim.setLoop(false);
    registerAnimation("broadcast_wave", waveAnim);

    Animation pulseAnim;
    for (int i = 0; i < 4; ++i) {
        pulseAnim.addFrame(sf::IntRect(i * 24, 240, 24, 24), frameTime_);
    }
    pulseAnim.setLoop(true);
    registerAnimation("egg_pulse", pulseAnim);
}

void AnimationSystem::createWalkAnimation(PlayerOrientation orientation) 
{
    Animation walkAnim;
    int yOffset = 0;
    switch (orientation) {
        case PlayerOrientation::SOUTH:
            yOffset = 0;
            break;
        case PlayerOrientation::WEST:
            yOffset = 32;
            break;
        case PlayerOrientation::EAST:
            yOffset = 64;
            break;
        case PlayerOrientation::NORTH:
            yOffset = 96;
            break;
    }
    for (int i = 0; i < 4; ++i) {
        walkAnim.addFrame(sf::IntRect(i * 32, yOffset, 32, 32), frameTime_);
    }
    walkAnim.setLoop(true);
    std::string animName = "player_walk_";
    switch (orientation) {
        case PlayerOrientation::NORTH:
            animName += "north";
            break;
        case PlayerOrientation::SOUTH:
            animName += "south";
            break;
        case PlayerOrientation::EAST:
            animName += "east";
            break;
        case PlayerOrientation::WEST:
            animName += "west";
            break;
    }
    registerAnimation(animName, walkAnim);
}

void AnimationSystem::createIdleAnimation(PlayerOrientation orientation) 
{
    Animation idleAnim;
    int yOffset = 0;
    switch (orientation) {
        case PlayerOrientation::SOUTH:
            yOffset = 0;
            break;
        case PlayerOrientation::WEST:
            yOffset = 32;
            break;
        case PlayerOrientation::EAST:
            yOffset = 64;
            break;
        case PlayerOrientation::NORTH:
            yOffset = 96;
            break;
    }
    idleAnim.addFrame(sf::IntRect(0, yOffset, 32, 32), frameTime_ * 8);
    idleAnim.addFrame(sf::IntRect(128, yOffset, 32, 32), frameTime_);
    idleAnim.setLoop(true);
    std::string animName = "player_idle_";
    switch (orientation) {
        case PlayerOrientation::NORTH:
            animName += "north";
            break;
        case PlayerOrientation::SOUTH:
            animName += "south";
            break;
        case PlayerOrientation::EAST:
            animName += "east";
            break;
        case PlayerOrientation::WEST:
            animName += "west";
            break;
    }
    registerAnimation(animName, idleAnim);
}

}
