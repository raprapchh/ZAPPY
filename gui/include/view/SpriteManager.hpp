#pragma once
#include "Common.hpp"
#include "utils/ResourceManager.hpp"
#include "model/Player.hpp"
#include "model/Egg.hpp"
namespace zappy {
struct SpriteBatch {
    sf::Texture* texture;
    std::vector<sf::Sprite> sprites;
    sf::RenderStates states;
};
class SpriteManager {
public:
    SpriteManager();
    ~SpriteManager() = default;

    bool initialize();

    sf::Sprite createPlayerSprite(int level, const sf::Color& teamColor, PlayerOrientation orientation);
    sf::Sprite createEggSprite(const sf::Color& teamColor, EggState state);
    sf::Sprite createResourceSprite(ResourceType type, int quantity);
    sf::Sprite createTileSprite(bool hasResources, bool highlighted);
    sf::Sprite createUISprite(const std::string& name);
    sf::Sprite createIconSprite(const std::string& iconName);

    void beginBatch();
    void addSprite(const sf::Sprite& sprite, sf::Texture* texture = nullptr);
    void endBatch(sf::RenderTarget& target);

    void setPlayerFrame(sf::Sprite& sprite, int frame, PlayerOrientation orientation);
    void setResourceColor(sf::Sprite& sprite, ResourceType type);
    void updateSpritePosition(sf::Sprite& sprite, float x, float y);

    sf::IntRect getPlayerRect(int level, PlayerOrientation orientation, int frame = 0);
    sf::IntRect getResourceRect(ResourceType type);
    sf::IntRect getUIRect(const std::string& name);

private:

    sf::Texture atlasTexture_;
    std::map<std::string, sf::IntRect> spriteRects_;
    std::map<ResourceType, sf::IntRect> resourceRects_;
    std::array<std::array<sf::IntRect, 4>, 8> playerRects_;
    std::map<ResourceType, sf::Color> resourceColors_;
    std::array<sf::Color, 8> levelColors_;
    std::vector<SpriteBatch> batches_;
    std::map<sf::Texture*, size_t> textureToIndex_;
    bool batchingActive_;
};
}
