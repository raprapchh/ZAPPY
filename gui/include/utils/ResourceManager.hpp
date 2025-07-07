#pragma once
#include "Common.hpp"
namespace zappy {
class ResourceManager {
public:
    static ResourceManager& getInstance();
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    bool loadAll();
    void unloadAll();

    bool loadTexture(const std::string& name, const std::string& filename);
    const sf::Texture& getTexture(const std::string& name) const;
    sf::Texture& getTexture(const std::string& name);
    bool hasTexture(const std::string& name) const;

    bool loadFont(const std::string& name, const std::string& filename);
    const sf::Font& getFont(const std::string& name) const;
    sf::Font& getFont(const std::string& name);
    bool hasFont(const std::string& name) const;

    template<typename T>
    T& get(const std::string& name);
    template<typename T>
    const T& get(const std::string& name) const;

private:
    ResourceManager() = default;
    void loadDefaultResources();
    void loadTextures();
    void loadFonts();
    void createDefaultTexture();
    void createAtlasTexture();

    std::map<std::string, sf::Texture> textures_;
    std::map<std::string, sf::Font> fonts_;
    sf::Texture defaultTexture_;
    sf::Font defaultFont_;
};

template<>
inline sf::Texture& ResourceManager::get<sf::Texture>(const std::string& name) 
{
    return getTexture(name);
}

template<>
inline const sf::Texture& ResourceManager::get<sf::Texture>(const std::string& name) const 
{
    return getTexture(name);
}

template<>
inline sf::Font& ResourceManager::get<sf::Font>(const std::string& name) 
{
    return getFont(name);
}

template<>
inline const sf::Font& ResourceManager::get<sf::Font>(const std::string& name) const 
{
    return getFont(name);
}
}
