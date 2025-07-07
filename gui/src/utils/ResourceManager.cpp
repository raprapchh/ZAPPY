#include "utils/ResourceManager.hpp"
#include "utils/Logger.hpp"
#include <stdexcept>

namespace zappy {

ResourceManager& ResourceManager::getInstance() 
{
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadAll() 
{
    LOG_INFO("Loading all resources...");
    try {
        loadDefaultResources();
        loadTextures();
        loadFonts();
        createDefaultTexture();
        createAtlasTexture();
        LOG_INFO("All resources loaded successfully");
        return true;
    } catch (const std::exception& e) {
        LOG_ERRORF("Failed to load resources: %s", e.what());
        return false;
    }
}

void ResourceManager::unloadAll() 
{
    LOG_INFO("Unloading all resources...");
    textures_.clear();
    fonts_.clear();
    LOG_INFO("All resources unloaded");
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) 
{
    if (hasTexture(name)) {
        LOG_DEBUGF("Texture '%s' already loaded", name.c_str());
        return true;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        LOG_ERRORF("Failed to load texture '%s' from file '%s'", name.c_str(), filename.c_str());
        return false;
    }
    textures_[name] = std::move(texture);
    LOG_DEBUGF("Loaded texture '%s' from '%s'", name.c_str(), filename.c_str());
    return true;
}

const sf::Texture& ResourceManager::getTexture(const std::string& name) const 
{
    auto it = textures_.find(name);
    if (it == textures_.end()) {
        LOG_ERRORF("Texture '%s' not found", name.c_str());
        auto defaultIt = textures_.find("default");
        if (defaultIt != textures_.end()) {
            return defaultIt->second;
        }
        throw std::runtime_error("Texture '" + name + "' not found and no default texture available");
    }
    return it->second;
}

sf::Texture& ResourceManager::getTexture(const std::string& name) 
{
    auto it = textures_.find(name);
    if (it == textures_.end()) {
        LOG_ERRORF("Texture '%s' not found", name.c_str());
        auto defaultIt = textures_.find("default");
        if (defaultIt != textures_.end()) {
            return defaultIt->second;
        }
        throw std::runtime_error("Texture '" + name + "' not found and no default texture available");
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string& name) const 
{
    return textures_.find(name) != textures_.end();
}

bool ResourceManager::loadFont(const std::string& name, const std::string& filename) 
{
    if (hasFont(name)) {
        LOG_DEBUGF("Font '%s' already loaded", name.c_str());
        return true;
    }
    sf::Font font;
    if (!font.loadFromFile(filename)) {
        LOG_ERRORF("Failed to load font '%s' from file '%s'", name.c_str(), filename.c_str());
        return false;
    }
    fonts_[name] = std::move(font);
    LOG_DEBUGF("Loaded font '%s' from '%s'", name.c_str(), filename.c_str());
    return true;
}

const sf::Font& ResourceManager::getFont(const std::string& name) const 
{
    auto it = fonts_.find(name);
    if (it == fonts_.end()) {
        LOG_ERRORF("Font '%s' not found", name.c_str());
        auto defaultIt = fonts_.find("default");
        if (defaultIt != fonts_.end()) {
            return defaultIt->second;
        }
        static sf::Font fallbackFont;
        static bool fallbackCreated = false;
        if (!fallbackCreated) {
            std::vector<std::string> fallbackPaths = {
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/System/Library/Fonts/Helvetica.ttc",
                "/System/Library/Fonts/Arial.ttf"
            };
            for (const auto& path : fallbackPaths) {
                if (fallbackFont.loadFromFile(path)) {
                    LOG_DEBUGF("Loaded fallback font from '%s'", path.c_str());
                    fallbackCreated = true;
                    break;
                }
            }
            if (!fallbackCreated) {
                LOG_WARNING("No fallback font could be loaded - text rendering may not work");
                fallbackCreated = true;
            }
        }
        return fallbackFont;
    }
    return it->second;
}

sf::Font& ResourceManager::getFont(const std::string& name) 
{
    auto it = fonts_.find(name);
    if (it == fonts_.end()) {
        LOG_ERRORF("Font '%s' not found", name.c_str());
        auto defaultIt = fonts_.find("default");
        if (defaultIt != fonts_.end()) {
            return defaultIt->second;
        }
        static sf::Font fallbackFont;
        static bool fallbackCreated = false;
        if (!fallbackCreated) {
            std::vector<std::string> fallbackPaths = {
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/System/Library/Fonts/Helvetica.ttc",
                "/System/Library/Fonts/Arial.ttf"
            };
            for (const auto& path : fallbackPaths) {
                if (fallbackFont.loadFromFile(path)) {
                    LOG_DEBUGF("Loaded fallback font from '%s'", path.c_str());
                    fallbackCreated = true;
                    break;
                }
            }
            if (!fallbackCreated) {
                LOG_WARNING("No fallback font could be loaded - text rendering may not work");
                fallbackCreated = true;
            }
        }
        return fallbackFont;
    }
    return it->second;
}

bool ResourceManager::hasFont(const std::string& name) const 
{
    return fonts_.find(name) != fonts_.end();
}

void ResourceManager::loadDefaultResources() 
{
    LOG_DEBUG("Loading default resources...");
    sf::Font defaultFont;
    if (defaultFont.loadFromFile("Share/OpenSans Regular.ttf")) {
        fonts_["default"] = std::move(defaultFont);
        LOG_DEBUGF("Loaded default font from 'Share/OpenSans Regular.ttf'", "");
        return;
    } else {
        LOG_WARNING("Could not load project font 'Share/OpenSans Regular.ttf'. Trying system fonts.");
    }
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation-mono/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (defaultFont.loadFromFile(path)) {
            fonts_["default"] = std::move(defaultFont);
            LOG_DEBUGF("Loaded default font from '%s'", path.c_str());
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded) {
        LOG_WARNING("Could not load any system font for default - text rendering may not work.");
    }
}

void ResourceManager::loadTextures() 
{
    LOG_DEBUG("Loading textures...");
    loadTexture("player", "gui/assets/sprites/player.png");
    loadTexture("egg", "gui/assets/sprites/egg.png");
    loadTexture("food", "gui/assets/sprites/food.png");
    loadTexture("grass", "gui/assets/sprites/grass.png");
    loadTexture("crystal_blue", "gui/assets/sprites/crystal-blue.png");
    loadTexture("crystal_green", "gui/assets/sprites/crystal-green.png");
    loadTexture("crystal_grey", "gui/assets/sprites/crystal-grey.png");
    loadTexture("crystal_orange", "gui/assets/sprites/crystal-orange.png");
    loadTexture("crystal_pink", "gui/assets/sprites/crystal-pink.png");
    loadTexture("crystal_yellow", "gui/assets/sprites/crystal-yellow.png");
    LOG_INFO("Game textures loaded successfully");
}

void ResourceManager::loadFonts() 
{
    LOG_DEBUG("Loading fonts...");
}

void ResourceManager::createDefaultTexture() 
{
    LOG_DEBUG("Creating default texture...");
    sf::Image defaultImage;
    defaultImage.create(32, 32, sf::Color::Magenta);
    sf::Texture defaultTexture;
    if (defaultTexture.loadFromImage(defaultImage)) {
        textures_["default"] = std::move(defaultTexture);
        LOG_DEBUG("Created default texture");
    } else {
        LOG_ERROR("Failed to create default texture");
    }
}

void ResourceManager::createAtlasTexture() 
{
    LOG_DEBUG("Creating atlas texture...");
    sf::Image atlasImage;
    atlasImage.create(256, 256, sf::Color::Transparent);
    for (unsigned int y = 0; y < 256; ++y) {
        for (unsigned int x = 0; x < 256; ++x) {
            if ((x / 32 + y / 32) % 2 == 0) {
                atlasImage.setPixel(x, y, sf::Color(100, 100, 100));
            }
        }
    }
    sf::Texture atlasTexture;
    if (atlasTexture.loadFromImage(atlasImage)) {
        textures_["atlas"] = std::move(atlasTexture);
        LOG_DEBUG("Created atlas texture");
    } else {
        LOG_ERROR("Failed to create atlas texture");
    }
}

}
