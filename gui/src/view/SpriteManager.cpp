#include "view/SpriteManager.hpp"
#include "utils/Logger.hpp"

namespace zappy {

SpriteManager::SpriteManager()
    : batchingActive_(false) 
    {
}

bool SpriteManager::initialize() 
{
    batches_.clear();
    textureToIndex_.clear();
    LOG_DEBUG("SpriteManager initialized");
    return true;
}

sf::Sprite SpriteManager::createPlayerSprite(int level, const sf::Color& teamColor, PlayerOrientation orientation) 
{
    (void)orientation;
    sf::Sprite sprite;
    sf::Texture& texture = ResourceManager::getInstance().getTexture("player");
    sprite.setTexture(texture);
    sf::Color playerColor = teamColor;
    float brightness = 0.5f + (static_cast<float>(level) / 8.0f) * 0.5f;
    playerColor.r = static_cast<sf::Uint8>(static_cast<float>(playerColor.r) * brightness);
    playerColor.g = static_cast<sf::Uint8>(static_cast<float>(playerColor.g) * brightness);
    playerColor.b = static_cast<sf::Uint8>(static_cast<float>(playerColor.b) * brightness);
    sprite.setColor(playerColor);
    sf::Vector2u textureSize = texture.getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    return sprite;
}

sf::Sprite SpriteManager::createEggSprite(const sf::Color& teamColor, EggState state) 
{
    (void)state;
    sf::Sprite sprite;
    sf::Texture& texture = ResourceManager::getInstance().getTexture("egg");
    sprite.setTexture(texture);
    sf::Color eggColor = teamColor;
    eggColor.a = 180;
    sprite.setColor(eggColor);
    sf::Vector2u textureSize = texture.getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    return sprite;
}

sf::Sprite SpriteManager::createResourceSprite(ResourceType type, int quantity) 
{
    sf::Sprite sprite;
    std::string textureName;
    switch (type) {
        case ResourceType::Food:
            textureName = "food";
            break;
        case ResourceType::Linemate:
            textureName = "crystal_grey";
            break;
        case ResourceType::Deraumere:
            textureName = "crystal_orange";
            break;
        case ResourceType::Sibur:
            textureName = "crystal_yellow";
            break;
        case ResourceType::Mendiane:
            textureName = "crystal_pink";
            break;
        case ResourceType::Phiras:
            textureName = "crystal_green";
            break;
        case ResourceType::Thystame:
            textureName = "crystal_blue";
            break;
        case ResourceType::Count:
            textureName = "crystal_grey";
            break;
    }
    sf::Texture& texture = ResourceManager::getInstance().getTexture(textureName);
    sprite.setTexture(texture);
    sf::Vector2u textureSize = texture.getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    float scale = 0.8f + (static_cast<float>(quantity) / 10.0f) * 0.4f;
    scale = std::min(scale, 1.5f);
    sprite.setScale(scale, scale);
    return sprite;
}

sf::Sprite SpriteManager::createTileSprite(bool hasResources, bool highlighted) 
{
    sf::Sprite sprite;
    sf::Texture& texture = ResourceManager::getInstance().getTexture("grass");
    sprite.setTexture(texture);
    sf::Color tileColor = sf::Color::White;
    if (hasResources) {
        tileColor = sf::Color(220, 255, 220);
    }
    if (highlighted) {
        tileColor = sf::Color(255, 255, 150);
    }
    sprite.setColor(tileColor);
    sf::Vector2u textureSize = texture.getSize();
    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
    return sprite;
}

sf::Sprite SpriteManager::createUISprite(const std::string& name) 
{
    sf::Sprite sprite;
    sf::Texture& texture = ResourceManager::getInstance().getTexture("default");
    sprite.setTexture(texture);
    if (name == "button") {
        sprite.setColor(sf::Color(100, 100, 100, 200));
    } else if (name == "panel") {
        sprite.setColor(sf::Color(50, 50, 50, 180));
    } else {
        sprite.setColor(sf::Color::White);
    }
    return sprite;
}

sf::Sprite SpriteManager::createIconSprite(const std::string& iconName) 
{
    sf::Sprite sprite;
    sf::Texture& texture = ResourceManager::getInstance().getTexture("default");
    sprite.setTexture(texture);
    if (iconName == "play") {
        sprite.setColor(sf::Color::Green);
    } else if (iconName == "pause") {
        sprite.setColor(sf::Color::Yellow);
    } else if (iconName == "stop") {
        sprite.setColor(sf::Color::Red);
    } else {
        sprite.setColor(sf::Color::White);
    }
    sprite.setScale(0.5f, 0.5f);
    return sprite;
}

void SpriteManager::beginBatch() 
{
    batchingActive_ = true;
    batches_.clear();
    textureToIndex_.clear();
}

void SpriteManager::addSprite(const sf::Sprite& sprite, sf::Texture* texture) 
{
    if (!batchingActive_) 
        return;
    if (!texture) {
        texture = const_cast<sf::Texture*>(sprite.getTexture());
    }
    if (!texture) 
        return;
    auto it = textureToIndex_.find(texture);
    size_t batchIndex;
    if (it != textureToIndex_.end()) {
        batchIndex = it->second;
    } else {
        batchIndex = batches_.size();
        textureToIndex_[texture] = batchIndex;
        SpriteBatch batch;
        batch.texture = texture;
        batch.states.texture = texture;
        batches_.push_back(batch);
    }
    batches_[batchIndex].sprites.push_back(sprite);
}

void SpriteManager::endBatch(sf::RenderTarget& target) 
{
    if (!batchingActive_) 
        return;
    for (const auto& batch : batches_) {
        for (const auto& sprite : batch.sprites) {
            target.draw(sprite, batch.states);
        }
    }
    batchingActive_ = false;
}

void SpriteManager::setPlayerFrame(sf::Sprite& sprite, int frame, PlayerOrientation orientation) 
{
    (void)sprite;
    (void)frame;
    (void)orientation;
}

void SpriteManager::setResourceColor(sf::Sprite& sprite, ResourceType type) 
{
    sf::Color resourceColor;
    switch (type) {
        case ResourceType::Food:
            resourceColor = sf::Color(139, 69, 19);
            break;
        case ResourceType::Linemate:
            resourceColor = sf::Color(128, 128, 128);
            break;
        case ResourceType::Deraumere:
            resourceColor = sf::Color(255, 165, 0);
            break;
        case ResourceType::Sibur:
            resourceColor = sf::Color(255, 255, 0);
            break;
        case ResourceType::Mendiane:
            resourceColor = sf::Color(128, 0, 128);
            break;
        case ResourceType::Phiras:
            resourceColor = sf::Color(0, 255, 0);
            break;
        case ResourceType::Thystame:
            resourceColor = sf::Color(0, 0, 255);
            break;
        case ResourceType::Count:
            resourceColor = sf::Color::White;
            break;
    }
    sprite.setColor(resourceColor);
}

void SpriteManager::updateSpritePosition(sf::Sprite& sprite, float x, float y) 
{
    sprite.setPosition(x, y);
}

sf::IntRect SpriteManager::getResourceRect(ResourceType type) 
{
    int size = 32;
    int index = static_cast<int>(type);
    return sf::IntRect(index * size, 0, size, size);
}

sf::IntRect SpriteManager::getPlayerRect(int level, PlayerOrientation orientation, int frame) 
{
    (void)level;
    (void)orientation;
    (void)frame;
    return sf::IntRect(0, 0, 32, 32);
}

sf::IntRect SpriteManager::getUIRect(const std::string& name) 
{
    (void)name;
    return sf::IntRect(0, 0, 32, 32);
}

}
