#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

namespace zappy {
bool Config::parseArgs(int argc, char** argv, Config& config) 
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            return false;
        }
        else if (arg == "-p" || arg == "--port") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -p requires a port number" << std::endl;
                return false;
            }
            if (!parsePort(argv[++i], config.serverPort)) {
                std::cerr << "Error: Invalid port number: " << argv[i] << std::endl;
                return false;
            }
        }
        else if (arg == "--host" || arg == "-h") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -h requires a hostname" << std::endl;
                return false;
            }
            config.serverHost = argv[++i];
        }
        else if (arg == "-r" || arg == "--resolution") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -r requires a resolution (WIDTHxHEIGHT)" << std::endl;
                return false;
            }
            if (!parseResolution(argv[++i], config.windowWidth, config.windowHeight)) {
                std::cerr << "Error: Invalid resolution format: " << argv[i] << std::endl;
                return false;
            }
        }
        else if (arg == "-f" || arg == "--fullscreen") {
            config.fullscreen = true;
        }
        else if (arg == "--no-vsync") {
            config.vsync = false;
        }
        else if (arg == "--fps-limit") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --fps-limit requires a number" << std::endl;
                return false;
            }
            try {
                config.fpsLimit = std::stoi(argv[++i]);
                if (config.fpsLimit <= 0) {
                    std::cerr << "Error: FPS limit must be positive" << std::endl;
                    return false;
                }
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid FPS limit: " << argv[i] << std::endl;
                return false;
            }
        }
        else if (arg == "--show-fps") {
            config.showFPS = true;
        }
        else if (arg == "--no-grid") {
            config.showGrid = false;
        }
        else if (arg == "--debug") {
            config.showDebugInfo = true;
        }
        else if (arg == "--no-antialiasing") {
            config.enableAntialiasing = false;
        }
        else if (arg == "--config") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --config requires a file path" << std::endl;
                return false;
            }
            if (!config.loadFromFile(argv[++i])) {
                std::cerr << "Error: Failed to load config file: " << argv[i] << std::endl;
                return false;
            }
        }
        else if (arg == "-g" || arg == "--graphics") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -g requires a graphics mode (2d or 3d)" << std::endl;
                return false;
            }
            std::string mode = argv[++i];
            if (mode == "2d") {
                config.renderMode = Config::RenderMode::Mode2D;
            } else if (mode == "3d") {
                config.renderMode = Config::RenderMode::Mode3D;
            } else {
                std::cerr << "Error: Invalid graphics mode. Use '2d' or '3d'" << std::endl;
                return false;
            }
        }
        else if (arg == "--assets-path") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --assets-path requires a directory path" << std::endl;
                return false;
            }
            config.assetsPath = argv[++i];
            if (!config.assetsPath.empty() && config.assetsPath.back() != '/') {
                config.assetsPath += '/';
            }
        }
        else {
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    return true;
}

void Config::printUsage(const char* programName) 
{
    std::cout << "USAGE: " << programName << " -p <port> -h <machine>\n"
              << "Options:\n"
              << "  -p, --port <port>    Port number for the server\n"
              << "  -h, --host <machine> Hostname or IP address of the server\n"
              << "  -g, --graphics <mode> Graphics mode: 2d or 3d (default: 2d)\n"
              << "  -r, --resolution <WxH> Window resolution\n"
              << "  -f, --fullscreen     Enable fullscreen mode\n"
              << "  --debug              Show debug information\n"
              << "  --help               Show this help message\n"
              << std::endl;
}

bool Config::validate() const 
{
    if (serverPort <= 0 || serverPort > 65535) {
        std::cerr << "Error: Invalid port number: " << serverPort << std::endl;
        return false;
    }
    if (serverHost.empty()) {
        std::cerr << "Error: Server host cannot be empty" << std::endl;
        return false;
    }
    if (windowWidth <= 0 || windowHeight <= 0) {
        std::cerr << "Error: Invalid window dimensions: " << windowWidth << "x" << windowHeight << std::endl;
        return false;
    }
    if (fpsLimit <= 0) {
        std::cerr << "Error: Invalid FPS limit: " << fpsLimit << std::endl;
        return false;
    }
    if (zoomSpeed <= 0.0f || panSpeed <= 0.0f) {
        std::cerr << "Error: Invalid camera settings" << std::endl;
        return false;
    }
    return true;
}

bool Config::loadFromFile(const std::string& filepath) 
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        if (key == "server_host") {
            serverHost = value;
        } else if (key == "server_port") {
            try {
                serverPort = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Warning: Invalid server_port in config: " << value << std::endl;
            }
        } else if (key == "window_width") {
            try {
                windowWidth = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Warning: Invalid window_width in config: " << value << std::endl;
            }
        } else if (key == "window_height") {
            try {
                windowHeight = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Warning: Invalid window_height in config: " << value << std::endl;
            }
        } else if (key == "fullscreen") {
            fullscreen = (value == "true" || value == "1");
        } else if (key == "vsync") {
            vsync = (value == "true" || value == "1");
        } else if (key == "fps_limit") {
            try {
                fpsLimit = std::stoi(value);
            } catch (const std::exception&) {
                std::cerr << "Warning: Invalid fps_limit in config: " << value << std::endl;
            }
        } else if (key == "show_fps") {
            showFPS = (value == "true" || value == "1");
        } else if (key == "show_grid") {
            showGrid = (value == "true" || value == "1");
        } else if (key == "debug") {
            showDebugInfo = (value == "true" || value == "1");
        } else if (key == "assets_path") {
            assetsPath = value;
            if (!assetsPath.empty() && assetsPath.back() != '/') {
                assetsPath += '/';
            }
        }
    }
    return true;
}

bool Config::parsePort(const std::string& str, int& port) 
{
    try {
        port = std::stoi(str);
        return port > 0 && port <= 65535;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseResolution(const std::string& str, int& width, int& height) 
{
    std::regex resolutionRegex(R"((\d+)x(\d+))");
    std::smatch match;
    if (std::regex_match(str, match, resolutionRegex)) {
        try {
            width = std::stoi(match[1].str());
            height = std::stoi(match[2].str());
            return width > 0 && height > 0;
        } catch (const std::exception&) {
            return false;
        }
    }
    return false;
}
}
