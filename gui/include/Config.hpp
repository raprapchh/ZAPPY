#pragma once
#include "Common.hpp"
namespace zappy {
class Config {
public:
    std::string serverHost;
    int serverPort;
    Duration networkTimeout{5000};
    int windowWidth = constants::DEFAULT_WINDOW_WIDTH;
    int windowHeight = constants::DEFAULT_WINDOW_HEIGHT;
    bool fullscreen = false;
    bool vsync = true;
    int fpsLimit = 60;
    std::string windowTitle = "Zappy GUI";
    float zoomSpeed = 1.2f;
    float panSpeed = 500.0f;
    float defaultZoom = 1.0f;
    float minZoom = 0.1f;
    float maxZoom = 10.0f;
    bool showFPS = true;
    bool showGrid = true;
    bool showDebugInfo = false;
    bool enableAntialiasing = true;
    enum class RenderMode {
        Mode2D,
        Mode3D
    };
    RenderMode renderMode = RenderMode::Mode2D;
    std::string assetsPath = "assets/";
    std::string fontsPath = "assets/fonts/";
    std::string texturesPath = "assets/textures/";
public:
    static bool parseArgs(int argc, char** argv, Config& config);
    static void printUsage(const char* programName);
    bool validate() const;
    bool loadFromFile(const std::string& filepath);

private:
    static bool parsePort(const std::string& str, int& port);
    static bool parseResolution(const std::string& str, int& width, int& height);
};
}
