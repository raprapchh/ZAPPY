#include "Common.hpp"
#include "Config.hpp"
#include "Application.hpp"
namespace zappy {

int main(int argc, char** argv) 
{
    try {
        Config config;
        if (!Config::parseArgs(argc, argv, config)) {
            Config::printUsage(argv[0]);
            return 1;
        }

        if (config.renderMode == Config::RenderMode::Mode3D && !config.fullscreen)
            config.fullscreen = true;

        if (!config.validate()) {
            std::cerr << "Error: Invalid configuration\n";
            return 1;
        }
        Application app(config);
        if (!app.initialize()) {
            std::cerr << "Error: Failed to initialize application\n";
            return 1;
        }
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: Unknown exception" << std::endl;
        return 1;
    }
}
}

int main(int argc, char** argv) 
{
    return zappy::main(argc, argv);
}
