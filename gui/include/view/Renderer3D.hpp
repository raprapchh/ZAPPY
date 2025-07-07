#pragma once
#include "IRenderer.hpp"
#include "model/World.hpp"
#include "view/Camera.hpp"
#include <raylib.h>
#include <raymath.h>
#include <optional>

#include "ui/UIManager.hpp"

#ifdef RAYLIB_AVAILABLE
#else
typedef struct Vector3 { float x, y, z; } Vector3;
typedef struct Vector2 { float x, y; } Vector2;
typedef struct Camera3D { Vector3 position, target, up; float fovy; int projection; } Camera3D;
typedef struct Model { void* dummy; } Model;
typedef struct Texture2D { void* dummy; } Texture2D;
typedef struct Color { unsigned char r, g, b, a; } Color;
#define CAMERA_PERSPECTIVE 0
#define WHITE (Color){255, 255, 255, 255}
#define GREEN (Color){0, 228, 48, 255}
#define YELLOW (Color){253, 249, 0, 255}
#define BLUE (Color){0, 121, 241, 255}
#define PINK (Color){255, 109, 194, 255}
#define ORANGE (Color){255, 161, 0, 255}
#define RED (Color){230, 41, 55, 255}
#define LIGHTGRAY (Color){200, 200, 200, 255}
#define SKYBLUE (Color){102, 191, 255, 255}
#define DARKGRAY (Color){80, 80, 80, 255}
#define PURPLE (Color){200, 122, 255, 255}
#define BEIGE (Color){211, 176, 131, 255}
#define BROWN (Color){127, 106, 79, 255}
#define GOLD (Color){255, 203, 0, 255}
#endif

#include <vector>
#include <unordered_map>

namespace zappy {

class Renderer3D : public IRenderer {
public:
    explicit Renderer3D(const Config& config, UIManager* uiManager = nullptr);
    ~Renderer3D() override;

    bool initialize() override;
    void shutdown() override;
    void render(const World& world, const Camera& camera, float deltaTime) override;
    void resize(unsigned int width, unsigned int height) override;
    void setQuality(int quality) override;
    void setShowGrid(bool show) override;
    void setShowDebug(bool show) override;
    bool isWindowReady() const override;
    bool shouldClose() const override;
    void handleEvents() override;

    bool handleKeyboardEvents();
    bool handleMouseEvents();

private:
    void setupLighting();
    void renderTerrain(const World& world);
    void renderResources(const World& world);
    void renderPlayers(const World& world, float deltaTime);
    void renderEggs(const World& world);
    void renderUI(const World& world);
    void renderDebugInfo(const World& world, float deltaTime);
    void renderShadow(const Vector3& position, float size = 1.0f);
    void renderTileInfo(const World& world);
    void renderPlayerInfo(const World& world);
    void handleTileSelection(const World& world);
    void handlePlayerSelection(const World& world);

    void updateCamera3D(const Camera& camera2D);
    Vector3 toVector3(float x, float y, float z = 0.0f) const;
    Color getResourceColor(ResourceType type) const;
    std::string getResourceName(ResourceType type) const;

    const Config& config_;
    Camera3D camera3D_;

    Model terrainModel_;
    Model playerModel_;
    Model crystalModel_;
    Model eggModel_;
    Model foodModel_;

    Vector3 lightDirection_;
    Color shadowColor_;

    Vector3 sunPosition_;

    int quality_;
    bool showGrid_;
    bool showDebug_;
    bool initialized_;
    bool shadowsEnabled_;
    bool manualCameraControl_;

    float frameTime_;
    int fps_;

    std::optional<sf::Vector2i> selectedTile_;
    std::optional<int> selectedPlayerId_;
    UIManager* uiManager_;
};

}
