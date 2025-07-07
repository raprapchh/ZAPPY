#pragma once
#include "Common.hpp"
#include "view/IRenderer.hpp"
#include "view/Renderer.hpp"
#include "view/Renderer3D.hpp"
#include "view/Camera.hpp"
#include "ui/UIManager.hpp"
#include "utils/Logger.hpp"
#include "network/Message.hpp"
#include "events/EventDispatcher.hpp"
#include <SFML/Audio.hpp>
namespace zappy {
class Application {
public:
    explicit Application(const Config& config);
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
    bool initialize();
    void run();
    void shutdown();
    EventDispatcher& getEventDispatcher() { return *eventDispatcher_; }
    void onWindowClosed();
    void onWindowResized(unsigned int width, unsigned int height);
    void onKeyPressed(sf::Keyboard::Key key);
    void onMouseButtonPressed(sf::Mouse::Button button, int x, int y);
    void onMouseMoved(int x, int y);
    void onMouseWheelScrolled(float delta, int x, int y);
private:
    void handleEvents();
    void handle3DEvents();
    void update(float deltaTime);
    void render();
    bool initializeSystems();
    void shutdownSystems();
    void processMessage(const Message& message);
    std::unique_ptr<EventDispatcher> eventDispatcher_;
    std::unique_ptr<NetworkClient> network_;
    std::unique_ptr<World> world_;
    std::unique_ptr<IRenderer> renderer_;
    std::unique_ptr<UIManager> ui_;
    std::unique_ptr<Camera> camera_;
    sf::RenderWindow window_;
    sf::Clock frameClock_;
    sf::Clock deltaClock_;
    const Config& config_;
    bool running_;
    bool initialized_;
    float targetFrameTime_;
    float frameTime_;
    float renderTime_;
    float updateTime_;
    sf::Clock performanceClock_;
    sf::Music music_;
    sf::SoundBuffer deathSoundBuffer_;
    sf::Sound deathSound_;
};
}
