#include "view/TileAnimationSystem.hpp"
#include "utils/Math.hpp"
#include <algorithm>
#include <cmath>

namespace zappy {

TileAnimationSystem::TileAnimationSystem() {
    circleShape_.setOrigin(16.0f, 16.0f);
    rectShape_.setSize(sf::Vector2f(32.0f, 32.0f));
}

void TileAnimationSystem::update(float deltaTime) {
    for (auto& anim : animations_) {
        anim->elapsed += deltaTime;
        switch (anim->type) {
            case TileAnimationType::BROADCAST_WAVE:
                anim->radius = static_cast<int>(anim->getProgress() * 10.0f * anim->intensity);
                break;
            default:
                break;
        }
    }
    
    animations_.erase(
        std::remove_if(animations_.begin(), animations_.end(),
                      [](const std::unique_ptr<TileAnimation>& anim) {
                          return anim->isFinished();
                      }),
        animations_.end()
    );
}

void TileAnimationSystem::render(sf::RenderTarget& target, const sf::View& view) {
    sf::FloatRect viewBounds = sf::FloatRect(
        view.getCenter().x - view.getSize().x / 2,
        view.getCenter().y - view.getSize().y / 2,
        view.getSize().x,
        view.getSize().y
    );
    
    for (const auto& anim : animations_) {
        sf::Vector2f worldPos(anim->position.x * 32.0f, anim->position.y * 32.0f);
        sf::FloatRect animBounds(worldPos.x - 160, worldPos.y - 160, 320, 320); 
        
        if (!viewBounds.intersects(animBounds)) {
            continue;
        }
        
        switch (anim->type) {
            case TileAnimationType::ELEVATION:
                renderElevationAnimation(target, *anim);
                break;
            case TileAnimationType::BROADCAST_WAVE:
                renderBroadcastWave(target, *anim);
                break;
            case TileAnimationType::DEATH:
                renderDeathAnimation(target, *anim);
                break;
        }
    }
}

void TileAnimationSystem::startElevationAnimation(sf::Vector2i position, int level, const sf::Color& teamColor) {
    auto anim = std::make_unique<TileAnimation>(position, TileAnimationType::ELEVATION, 2.0f, level);
    anim->color = teamColor;
    animations_.push_back(std::move(anim));
}

void TileAnimationSystem::startBroadcastWave(sf::Vector2i position, float intensity, const sf::Color& teamColor) {
    auto anim = std::make_unique<TileAnimation>(position, TileAnimationType::BROADCAST_WAVE, 3.0f, intensity);
    anim->color = teamColor;
    animations_.push_back(std::move(anim));
}

void TileAnimationSystem::startDeathAnimation(sf::Vector2i position) {
    auto anim = std::make_unique<TileAnimation>(position, TileAnimationType::DEATH, 2.0f, 1.0f);
    anim->color = sf::Color::Red;
    animations_.push_back(std::move(anim));
}

void TileAnimationSystem::renderElevationAnimation(sf::RenderTarget& target, const TileAnimation& anim) {
    sf::Vector2f centerPos(anim.position.x * 32.0f + 16.0f, anim.position.y * 32.0f + 16.0f);
    float progress = anim.getProgress();
    float intensity = anim.intensity;
    float pulseScale = 1.0f + 0.3f * std::sin(progress * 6.28f * 3.0f) * (1.0f - progress);
    rectShape_.setSize(sf::Vector2f(32.0f * pulseScale, 32.0f * pulseScale));
    rectShape_.setOrigin(16.0f * pulseScale, 16.0f * pulseScale);
    rectShape_.setPosition(centerPos);
    
    sf::Color color = anim.color;
    color.a = static_cast<sf::Uint8>(200 * (1.0f - progress));
    rectShape_.setFillColor(color);
    rectShape_.setOutlineColor(sf::Color(255, 215, 0, color.a));
    rectShape_.setOutlineThickness(2.0f);
    target.draw(rectShape_);
    int numRings = static_cast<int>(intensity) + 1;
    for (int ring = 0; ring < numRings; ++ring) {
        float ringDelay = ring * 0.3f;
        float ringProgress = std::max(0.0f, std::min(1.0f, (progress - ringDelay) / (1.0f - ringDelay)));
        
        if (ringProgress <= 0.0f) continue;
        
        std::vector<sf::Vector2i> adjacentTiles = {
            {-1, -1}, {0, -1}, {1, -1},
            {-1,  0},          {1,  0},
            {-1,  1}, {0,  1}, {1,  1}
        };
        
        for (const auto& offset : adjacentTiles) {
            sf::Vector2f adjPos(
                (anim.position.x + offset.x * (ring + 1)) * 32.0f + 16.0f,
                (anim.position.y + offset.y * (ring + 1)) * 32.0f + 16.0f
            );
            
            float tileAlpha = 150 * (1.0f - ringProgress) * (1.0f - progress);
            if (tileAlpha < 5.0f) continue;
            
            rectShape_.setSize(sf::Vector2f(32.0f, 32.0f));
            rectShape_.setOrigin(16.0f, 16.0f);
            rectShape_.setPosition(adjPos);
            
            sf::Color adjColor = sf::Color(255, 215, 0, static_cast<sf::Uint8>(tileAlpha));
            rectShape_.setFillColor(adjColor);
            rectShape_.setOutlineColor(sf::Color::Transparent);
            rectShape_.setOutlineThickness(0.0f);
            target.draw(rectShape_);
        }
    }
}

void TileAnimationSystem::renderBroadcastWave(sf::RenderTarget& target, const TileAnimation& anim) {
    sf::Vector2f centerPos(anim.position.x * 32.0f + 16.0f, anim.position.y * 32.0f + 16.0f);
    float progress = anim.getProgress();
    float maxRadius = 320.0f * anim.intensity;
    
    for (int wave = 0; wave < 3; ++wave) {
        float waveDelay = wave * 0.25f;
        float waveProgress = std::max(0.0f, std::min(1.0f, (progress - waveDelay) / (1.0f - waveDelay)));
        
        if (waveProgress <= 0.0f) continue;
        
        float baseRadius = 32.0f;
        float currentRadius = baseRadius + waveProgress * (maxRadius - baseRadius);
        float thickness = 6.0f * (1.0f - waveProgress);
        
        if (thickness < 1.0f) continue;
        
        circleShape_.setRadius(currentRadius);
        circleShape_.setOrigin(currentRadius, currentRadius);
        circleShape_.setPosition(centerPos);
        circleShape_.setFillColor(sf::Color::Transparent);
        
        sf::Color waveColor = anim.color;
        waveColor.a = static_cast<sf::Uint8>(120 * (1.0f - waveProgress));
        circleShape_.setOutlineColor(waveColor);
        circleShape_.setOutlineThickness(thickness);
        target.draw(circleShape_);
    }
}

void TileAnimationSystem::renderDeathAnimation(sf::RenderTarget& target, const TileAnimation& anim) {
    sf::Vector2f centerPos(anim.position.x * 32.0f + 16.0f, anim.position.y * 32.0f + 16.0f);
    float progress = anim.getProgress();
    
    float pulseScale = 1.0f + 0.5f * std::sin(progress * 6.28f * 4.0f) * (1.0f - progress);
    rectShape_.setSize(sf::Vector2f(32.0f * pulseScale, 32.0f * pulseScale));
    rectShape_.setOrigin(16.0f * pulseScale, 16.0f * pulseScale);
    rectShape_.setPosition(centerPos);
    
    sf::Color redColor = sf::Color::Red;
    redColor.a = static_cast<sf::Uint8>(255 * (1.0f - progress));
    rectShape_.setFillColor(redColor);
    rectShape_.setOutlineColor(sf::Color(139, 0, 0, redColor.a));
    rectShape_.setOutlineThickness(2.0f);
    target.draw(rectShape_);
    
    if (progress < 0.8f) {
        sf::RectangleShape crossH(sf::Vector2f(24.0f, 4.0f));
        sf::RectangleShape crossV(sf::Vector2f(4.0f, 24.0f));
        
        crossH.setOrigin(12.0f, 2.0f);
        crossV.setOrigin(2.0f, 12.0f);
        crossH.setPosition(centerPos);
        crossV.setPosition(centerPos);
        
        sf::Color crossColor = sf::Color(200, 0, 0, static_cast<sf::Uint8>(255 * (0.8f - progress) / 0.8f));
        crossH.setFillColor(crossColor);
        crossV.setFillColor(crossColor);
        
        target.draw(crossH);
        target.draw(crossV);
    }
}

void TileAnimationSystem::clear() {
    animations_.clear();
}

size_t TileAnimationSystem::getActiveAnimationCount() const {
    return animations_.size();
}

}
