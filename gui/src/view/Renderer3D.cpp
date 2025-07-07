#include "view/Renderer3D.hpp"
#include "Config.hpp"
#include "model/World.hpp"
#include "model/Player.hpp"
#include "model/Egg.hpp"
#include "model/Tile.hpp"
#include <iostream>
#include <cmath>
#include <raylib.h>
#include <raymath.h>

#ifdef RAYLIB_AVAILABLE
#else
[[maybe_unused]] static void SetTraceLogLevel(int) {}
[[maybe_unused]] static void InitWindow(int, int, const char*) {}
[[maybe_unused]] static bool IsWindowReady() { return false; }
[[maybe_unused]] static void CloseWindow() {}
[[maybe_unused]] static void SetTargetFPS(int) {}
#define LOG_NONE 7
#define KEY_G 71
#define KEY_R 82
#define MOUSE_BUTTON_LEFT 0
#define MOUSE_BUTTON_RIGHT 1
[[maybe_unused]] static bool WindowShouldClose() { return true; }
[[maybe_unused]] static void BeginDrawing() {}
[[maybe_unused]] static void EndDrawing() {}
[[maybe_unused]] static void ClearBackground(Color) {}
[[maybe_unused]] static void BeginMode3D(Camera3D) {}
[[maybe_unused]] static void EndMode3D() {}
[[maybe_unused]] static void (int, float) {}
[[maybe_unused]] static void DrawText(const char*, int, int, int, Color) {}
[[maybe_unused]] static void DrawRectangle(int, int, int, int, Color) {}
[[maybe_unused]] static void DrawCircle(int, int, int, Color) {}
[[maybe_unused]] static bool IsKeyDown(int) { return false; }
[[maybe_unused]] static float GetMouseWheelMove() { return 0.0f; }
[[maybe_unused]] static float GetTime() { return 0.0f; }
[[maybe_unused]] static int GetFPS() { return 60; }
[[maybe_unused]] static int GetScreenWidth() { return 800; }
[[maybe_unused]] static int GetScreenHeight() { return 600; }
[[maybe_unused]] static void SetWindowSize(int, int) {}
[[maybe_unused]] static bool FileExists(const char*) { return false; }
[[maybe_unused]] static Model LoadModelFromMesh(int) { return Model{nullptr}; }
[[maybe_unused]] static int GenMeshCube(float, float, float) { return 0; }
[[maybe_unused]] static int GenMeshSphere(float, int, int) { return 0; }
[[maybe_unused]] static int GenMeshPlane(float, float, int, int) { return 0; }
[[maybe_unused]] static void UnloadModel(Model) {}
[[maybe_unused]] static void DrawModel(Model, Vector3, float, Color) {}
[[maybe_unused]] static void DrawModelEx(Model, Vector3, Vector3, float, Vector3, Color) {}
[[maybe_unused]] static void DrawCube(Vector3, float, float, float, Color) {}
[[maybe_unused]] static void DrawCubeWires(Vector3, float, float, float, Color) {}
[[maybe_unused]] static void DrawSphere(Vector3, float, Color) {}
[[maybe_unused]] static void DrawSphereWires(Vector3, float, int, int, Color) {}
[[maybe_unused]] static void DrawCylinder(Vector3, float, float, float, int, Color) {}
[[maybe_unused]] static void DrawCylinderWires(Vector3, float, float, float, int, Color) {}
[[maybe_unused]] static Vector3 Vector3Subtract(Vector3 a, Vector3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
[[maybe_unused]] static Vector3 Vector3Normalize(Vector3 v) { return v; }
[[maybe_unused]] static Vector3 Vector3Scale(Vector3 v, float s) { return {v.x*s, v.y*s, v.z*s}; }
[[maybe_unused]] static float Vector3Length(Vector3) { return 1.0f; }
[[maybe_unused]] static Color ColorAlpha(Color c, float) { return c; }
typedef struct Image { void* dummy; } Image;
#define BLACK (Color){0, 0, 0, 255}
[[maybe_unused]] static Vector2 GetMouseDelta() { return {0.0f, 0.0f}; }
[[maybe_unused]] static bool IsMouseButtonDown(int) { return false; }
[[maybe_unused]] static bool IsMouseButtonPressed(int) { return false; }
[[maybe_unused]] static bool IsKeyPressed(int) { return false; }
[[maybe_unused]] static RayGetCollision GetRayCollisionQuad(Ray, Vector3, Vector3, Vector3, Vector3) { return RayGetCollision{false}; }
[[maybe_unused]] static RayCollision GetRayCollisionBox(Ray, BoundingBox) { return RayCollision{false}; }
[[maybe_unused]] static void DrawRectangleRounded(Rectangle, float, int, Color) {}
[[maybe_unused]] static void DrawRectangleRoundedLines(Rectangle, float, int, Color) {}
[[maybe_unused]] static bool CheckCollisionPointRec(Vector2, Rectangle) { return false; }
[[maybe_unused]] static void DrawLine(int, int, int, int, Color) {}
#endif

namespace zappy {

Renderer3D::Renderer3D(const Config& config, UIManager* uiManager)
    : config_(config)
    , lightDirection_({-1.0f, -1.0f, -1.0f})
    , shadowColor_({0, 0, 0, 100})
    , quality_(1)
    , showGrid_(true)
    , showDebug_(false)
    , initialized_(false)
    , shadowsEnabled_(true)
    , manualCameraControl_(false)
    , frameTime_(0.0f)
    , fps_(0)
    , uiManager_(uiManager)
{
}

Renderer3D::~Renderer3D() {
    shutdown();
}

bool Renderer3D::initialize() {
    if (initialized_) {
        return true;
    }

#ifndef RAYLIB_AVAILABLE
    std::cerr << "3D mode requested but Raylib is not available. Please install Raylib and recompile with -DRAYLIB_AVAILABLE" << std::endl;
    return false;
#endif

    SetTraceLogLevel(LOG_NONE);

    int windowWidth = config_.windowWidth;
    int windowHeight = config_.windowHeight;

    if (config_.fullscreen) {
        windowWidth = GetMonitorWidth(GetCurrentMonitor());
        windowHeight = GetMonitorHeight(GetCurrentMonitor());
    }

    InitWindow(windowWidth, windowHeight, config_.windowTitle.c_str());

    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize Raylib window" << std::endl;
        return false;
    }

    camera3D_.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera3D_.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera3D_.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera3D_.fovy = 45.0f;
    camera3D_.projection = CAMERA_PERSPECTIVE;

    setupLighting();

    if (FileExists("gui/assets/sprites3D/character-male-d.glb")) {
        playerModel_ = LoadModel("gui/assets/sprites3D/character-male-d.glb");
    } else {
        playerModel_ = LoadModelFromMesh(GenMeshCube(1.0f, 2.0f, 1.0f));
    }

    if (FileExists("gui/assets/sprites3D/Crystal.glb")) {
        crystalModel_ = LoadModel("gui/assets/sprites3D/Crystal.glb");
    } else {
        crystalModel_ = LoadModelFromMesh(GenMeshSphere(0.3f, 12, 12));
    }

    if (FileExists("gui/assets/sprites3D/Floor Grass Sliced B.glb")) {
        terrainModel_ = LoadModel("gui/assets/sprites3D/Floor Grass Sliced B.glb");
    } else {
        terrainModel_ = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));
    }

    if (FileExists("gui/assets/sprites3D/Egg.glb")) {
        eggModel_ = LoadModel("gui/assets/sprites3D/Egg.glb");
    } else {
        eggModel_ = LoadModelFromMesh(GenMeshSphere(0.4f, 16, 16));
    }

    if (FileExists("gui/assets/sprites3D/Hamburger.glb")) {
        foodModel_ = LoadModel("gui/assets/sprites3D/Hamburger.glb");
    } else {
        foodModel_ = LoadModelFromMesh(GenMeshSphere(0.2f, 12, 12));
    }

    initialized_ = true;
    return true;
}

void Renderer3D::shutdown() {
    if (!initialized_) {
        return;
    }

    UnloadModel(playerModel_);
    UnloadModel(crystalModel_);
    UnloadModel(terrainModel_);
    UnloadModel(eggModel_);
    UnloadModel(foodModel_);

    CloseWindow();
    initialized_ = false;
}

void Renderer3D::render(const World& world, const Camera& camera, float deltaTime) {
    if (!initialized_) {
        return;
    }

    frameTime_ = deltaTime;
    fps_ = GetFPS();

    handlePlayerSelection(world);
    handleTileSelection(world);

    if (!manualCameraControl_) {
        updateCamera3D(camera);
    }

    BeginDrawing();
    ClearBackground(SKYBLUE);

    BeginMode3D(camera3D_);

    renderTerrain(world);
    renderResources(world);
    renderPlayers(world, deltaTime);
    renderEggs(world);

    EndMode3D();

    renderPlayerInfo(world);
    renderTileInfo(world);

    if (showDebug_) {
        renderDebugInfo(world, deltaTime);
    }

    EndDrawing();
}

void Renderer3D::resize(unsigned int width, unsigned int height) {
    if (!initialized_) {
        return;
    }
    SetWindowSize(width, height);
}

void Renderer3D::setQuality(int quality) {
    quality_ = std::clamp(quality, 0, 3);
}

void Renderer3D::setShowGrid(bool show) {
    showGrid_ = show;
}

void Renderer3D::setShowDebug(bool show) {
    showDebug_ = show;
}

bool Renderer3D::isWindowReady() const {
    return initialized_ && IsWindowReady();
}

bool Renderer3D::shouldClose() const {
    return WindowShouldClose();
}

void Renderer3D::handleEvents() {
    UpdateCamera(&camera3D_, CAMERA_FREE);
    manualCameraControl_ = true;
    handleKeyboardEvents();
    handleMouseEvents();
}

bool Renderer3D::handleKeyboardEvents() {

    if (IsKeyPressed(KEY_G)) {
        showGrid_ = !showGrid_;
        return true;
    }

    if (IsKeyPressed(KEY_R)) {
        camera3D_.position = (Vector3){ 0.0f, 10.0f, 10.0f };
        camera3D_.target = (Vector3){ 0.0f, 0.0f, 0.0f };
        manualCameraControl_ = false;
        return true;
    }
    return false;
}

void Renderer3D::handlePlayerSelection(const World& world) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Ray ray = GetMouseRay(GetMousePosition(), camera3D_);
        float closestCollision = -1.0f;
        std::optional<int> newSelectedPlayerId;

        for (const auto& pair : world.getPlayers()) {
            const Player* player = pair.second.get();
            if (!player) continue;

            sf::Vector2f pos2D = player->getInterpolatedPosition();
            Vector3 position = toVector3(pos2D.x / 32.0f, 1.0f, pos2D.y / 32.0f);
            BoundingBox playerBox = {
                (Vector3){ position.x - 0.5f, position.y - 1.0f, position.z - 0.5f },
                (Vector3){ position.x + 0.5f, position.y + 1.0f, position.z + 0.5f }
            };

            RayCollision collision = GetRayCollisionBox(ray, playerBox);
            if (collision.hit) {
                if (closestCollision < 0 || collision.distance < closestCollision) {
                    closestCollision = collision.distance;
                    newSelectedPlayerId = player->getId();
                }
            }
        }

        if (newSelectedPlayerId) {
            selectedPlayerId_ = newSelectedPlayerId;
            selectedTile_ = std::nullopt;
            if (uiManager_) {
                const Player* player = world.getPlayer(*newSelectedPlayerId);
                if (player) {
                    uiManager_->showPlayerInfoPanel(player->getId(), *player);
                }
            }
        }
    }
}

void Renderer3D::handleTileSelection(const World& world) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (selectedTile_) {
            int panelWidth = 300;
            int panelHeight = 180;
            int panelX = GetScreenWidth() / 2 - panelWidth / 2;
            int panelY = GetScreenHeight() - 220;
            Rectangle panelRect = { (float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight };

            if (CheckCollisionPointRec(GetMousePosition(), panelRect)) {
                return;
            }
        }

        Ray ray = GetMouseRay(GetMousePosition(), camera3D_);
        sf::Vector2i mapSize = world.getMapSize();
        float closestCollision = -1.0f;
        std::optional<sf::Vector2i> newSelectedTile;

        for (int y = 0; y < mapSize.y; ++y) {
            for (int x = 0; x < mapSize.x; ++x) {
                Vector3 p1 = toVector3(x - 0.5f, 0, y - 0.5f);
                Vector3 p2 = toVector3(x + 0.5f, 0, y - 0.5f);
                Vector3 p3 = toVector3(x + 0.5f, 0, y + 0.5f);
                Vector3 p4 = toVector3(x - 0.5f, 0, y + 0.5f);

                RayCollision collision = GetRayCollisionQuad(ray, p1, p2, p3, p4);

                if (collision.hit) {
                    if (closestCollision < 0 || collision.distance < closestCollision) {
                        closestCollision = collision.distance;
                        newSelectedTile = sf::Vector2i(x, y);
                    }
                }
            }
        }
        if (newSelectedTile) {
            selectedTile_ = newSelectedTile;
            selectedPlayerId_ = std::nullopt;
            if (uiManager_) {
                const Tile& tile = world.getTile(newSelectedTile->x, newSelectedTile->y);
                uiManager_->showTileInfoPanel(newSelectedTile->x, newSelectedTile->y, tile);
            }
        }
    }
}

bool Renderer3D::handleMouseEvents() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector3 direction = Vector3Subtract(camera3D_.target, camera3D_.position);
        float distance = Vector3Length(direction);
        direction = Vector3Normalize(direction);

        distance += wheel * 2.0f;
        distance = std::max(2.0f, std::min(distance, 50.0f));

        camera3D_.position = Vector3Subtract(camera3D_.target, Vector3Scale(direction, distance));
        return true;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseDelta = GetMouseDelta();
        if (mouseDelta.x != 0 || mouseDelta.y != 0) {
            float sensitivity = 0.05f;

            Vector3 forward = Vector3Normalize(Vector3Subtract(camera3D_.target, camera3D_.position));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera3D_.up));
            Vector3 up = Vector3CrossProduct(right, forward);

            Vector3 panOffset = Vector3Add(
                Vector3Scale(right, -mouseDelta.x * sensitivity),
                Vector3Scale(up, mouseDelta.y * sensitivity)
            );

            camera3D_.position = Vector3Add(camera3D_.position, panOffset);
            camera3D_.target = Vector3Add(camera3D_.target, panOffset);

            return true;
        }
    }
    return false;
}

void Renderer3D::setupLighting() {
    sunPosition_ = (Vector3){ 10.0f, 20.0f, 10.0f };
    lightDirection_ = Vector3Normalize((Vector3){ -1.0f, -2.0f, -1.0f });
    shadowColor_ = (Color){ 0, 0, 0, 100 };
}

void Renderer3D::renderTerrain(const World& world) {
    sf::Vector2i mapSize = world.getMapSize();

    for (int y = 0; y < mapSize.y; ++y) {
        for (int x = 0; x < mapSize.x; ++x) {
            Vector3 position = toVector3(static_cast<float>(x), 0.0f, static_cast<float>(y));

            if (shadowsEnabled_) {
                renderShadow(position, 1.0f);
            }

            if (terrainModel_.meshes != nullptr) {
                DrawModel(terrainModel_, position, 1.0f, WHITE);
            } else {
                DrawCube(position, 1.0f, 0.1f, 1.0f, GREEN);
            }

            if (showGrid_) {
                DrawCubeWires(position, 1.0f, 0.1f, 1.0f, DARKGRAY);
            }
        }
    }
}

void Renderer3D::renderResources(const World& world) {
    sf::Vector2i mapSize = world.getMapSize();

    for (int y = 0; y < mapSize.y; ++y) {
        for (int x = 0; x < mapSize.x; ++x) {
            const Tile& tile = world.getTile(x, y);
            const auto& resources = tile.getResources();

            int resourceIndex = 0;
            for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
                if (resources[i] > 0) {
                    ResourceType type = static_cast<ResourceType>(i);

                    float offsetX = (resourceIndex % 3 - 1) * 0.3f;
                    float offsetZ = (resourceIndex / 3 - 1) * 0.3f;

                    Vector3 position = toVector3(
                        static_cast<float>(x) + offsetX,
                        0.01f,
                        static_cast<float>(y) + offsetZ
                    );

                    if (shadowsEnabled_) {
                        renderShadow(position, 0.4f);
                    }

                    if (type == ResourceType::Food) {
                        if (foodModel_.meshes != nullptr) {
                            DrawModel(foodModel_, position, 0.02f, WHITE);
                        } else {
                            DrawSphere(position, 0.2f, getResourceColor(type));
                        }
                    } else {
                        if (crystalModel_.meshes != nullptr) {
                            DrawModel(crystalModel_, position, 0.02f, getResourceColor(type));
                        } else {
                            DrawSphere(position, 0.2f, getResourceColor(type));
                        }
                    }

                    resourceIndex++;
                }
            }
        }
    }
}

void Renderer3D::renderPlayers(const World& world, float ) {
    const auto& players = world.getPlayers();

    for (const auto& pair : players) {
        const Player* player = pair.second.get();
        if (!player) continue;

        sf::Vector2f pos2D = player->getInterpolatedPosition();
        Vector3 position = toVector3(pos2D.x / 32.0f, 0.5f, pos2D.y / 32.0f);

        sf::Color teamColor = player->getTeamColor();
        Color color = { teamColor.r, teamColor.g, teamColor.b, teamColor.a };

        position.y += sinf(GetTime() * 3.0f + player->getId()) * 0.1f;

        if (shadowsEnabled_) {
            Vector3 shadowPos = position;
            shadowPos.y = 0.05f;
            renderShadow(shadowPos, 1.5f);
        }

        if (playerModel_.meshes != nullptr) {
            DrawModelEx(playerModel_, position, (Vector3){0.0f, 1.0f, 0.0f}, 180.0f, (Vector3){0.8f, 0.8f, 0.8f}, color);
        } else {
            DrawCube(position, 1.0f, 2.0f, 1.0f, WHITE);
        }

        Vector3 basePosition = position;
        basePosition.y = 0.1f;
        DrawCylinder(basePosition, 0.5f, 0.5f, 0.2f, 16, color);

        if (player->isInIncantation()) {
            float radius = 2.0f + sinf(GetTime() * 5.0f) * 0.5f;
            DrawCylinderWires(position, radius, radius, 0.1f, 12, PURPLE);
        }
    }
}

void Renderer3D::renderEggs(const World& world) {
    const auto& eggs = world.getEggs();

    for (const auto& pair : eggs) {
        const Egg* egg = pair.second.get();
        if (!egg) continue;

        sf::Vector2f pos2D = egg->getPosition();
        Vector3 position = toVector3(pos2D.x / 32.0f, 0.3f, pos2D.y / 32.0f);

        position.y += sinf(GetTime() * 2.0f + egg->getId()) * 0.05f;

        if (shadowsEnabled_) {
            Vector3 shadowPos = position;
            shadowPos.y = 0.05f;
            renderShadow(shadowPos, 0.8f);
        }

        if (eggModel_.meshes != nullptr) {
            DrawModel(eggModel_, position, 1.0f, WHITE);
        } else {
            DrawSphere(position, 0.4f, BEIGE);
            DrawSphereWires(position, 0.4f, 8, 8, BROWN);
        }
    }
}

void Renderer3D::renderUI(const World& world) {
    const auto& players = world.getPlayers();
    if (players.empty()) return;

    int panelY = 20;
    DrawRectangle(GetScreenWidth() - 320, panelY, 300, 80 + players.size() * 30, ColorAlpha(BLACK, 0.7f));
    DrawText("Players Info", GetScreenWidth() - 310, panelY + 10, 20, WHITE);

    int index = 0;
    for (const auto& pair : players) {
        const Player* player = pair.second.get();
        if (!player) continue;

        int yPos = panelY + 40 + index * 30;
        sf::Color teamColor = player->getTeamColor();
        Color color = { teamColor.r, teamColor.g, teamColor.b, 255 };

        DrawCircle(GetScreenWidth() - 300, yPos + 10, 8, color);

        std::string playerInfo = "ID:" + std::to_string(player->getId()) +
                                " Lvl:" + std::to_string(player->getLevel()) +
                                " Food:" + std::to_string(player->getResource(ResourceType::Food));

        DrawText(playerInfo.c_str(), GetScreenWidth() - 280, yPos, 16, WHITE);
        index++;
    }
}

void Renderer3D::renderDebugInfo(const World& world, float) {
    DrawText(("FPS: " + std::to_string(fps_)).c_str(), 10, 10, 20, WHITE);
    DrawText(("Frame Time: " + std::to_string(frameTime_ * 1000.0f) + "ms").c_str(), 10, 35, 20, WHITE);
    DrawText(("Players: " + std::to_string(world.getPlayers().size())).c_str(), 10, 60, 20, WHITE);
    DrawText(("Eggs: " + std::to_string(world.getEggs().size())).c_str(), 10, 85, 20, WHITE);

    std::string camInfo = "Camera: (" +
                         std::to_string(camera3D_.position.x) + ", " +
                         std::to_string(camera3D_.position.y) + ", " +
                         std::to_string(camera3D_.position.z) + ")";
    DrawText(camInfo.c_str(), 10, 110, 20, WHITE);
}

void Renderer3D::renderPlayerInfo(const World& world) {
    if (!selectedPlayerId_) {
        return;
    }

    const Player* player = world.getPlayer(*selectedPlayerId_);
    if (!player) {
        return;
    }

    int panelWidth = 300;
    int panelHeight = 150 + (static_cast<int>(ResourceType::Count) * 20);
    int panelX = GetScreenWidth() - panelWidth - 10;
    int panelY = 10;
    Rectangle panelRect = { (float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight };

    float buttonSize = 20.0f;
    Rectangle closeButtonRect = { panelRect.x + panelRect.width - buttonSize - 10, panelRect.y + 10, buttonSize, buttonSize };

    bool mouseOverClose = CheckCollisionPointRec(GetMousePosition(), closeButtonRect);
    if (mouseOverClose && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        selectedPlayerId_ = std::nullopt;
        return;
    }

    DrawRectangleRounded(panelRect, 0.1f, 10, ColorAlpha(BLACK, 0.8f));
    DrawRectangleRoundedLines(panelRect, 0.1f, 10, WHITE);

    std::string title = "Player #" + std::to_string(*selectedPlayerId_);
    DrawText(title.c_str(), panelX + 10, panelY + 10, 20, WHITE);

    DrawLine(closeButtonRect.x, closeButtonRect.y, closeButtonRect.x + closeButtonRect.width, closeButtonRect.y + closeButtonRect.height, mouseOverClose ? RED : WHITE);
    DrawLine(closeButtonRect.x, closeButtonRect.y + closeButtonRect.height, closeButtonRect.x + closeButtonRect.width, closeButtonRect.y, mouseOverClose ? RED : WHITE);

    int yPos = panelY + 40;
    std::string levelInfo = "Level: " + std::to_string(player->getLevel());
    DrawText(levelInfo.c_str(), panelX + 10, yPos, 16, WHITE);
    yPos += 20;

    std::string teamInfo = "Team: " + player->getTeamName();
    DrawText(teamInfo.c_str(), panelX + 10, yPos, 16, WHITE);
    yPos += 30;

    DrawText("Inventory:", panelX + 10, yPos, 18, WHITE);
    yPos += 25;

    for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
        ResourceType type = static_cast<ResourceType>(i);
        int count = player->getResource(type);
        DrawCircle(panelX + 20, yPos + 8, 8, getResourceColor(type));
        std::string resourceInfo = getResourceName(type) + ": " + std::to_string(count);
        DrawText(resourceInfo.c_str(), panelX + 40, yPos, 16, WHITE);
        yPos += 20;
    }
}

void Renderer3D::renderTileInfo(const World& world) {
    if (!selectedTile_) {
        return;
    }

    int panelWidth = 300;
    int panelHeight = 60 + (static_cast<int>(ResourceType::Count) * 20);
    int panelX = GetScreenWidth() - panelWidth - 10;
    int panelY = 10;
    Rectangle panelRect = { (float)panelX, (float)panelY, (float)panelWidth, (float)panelHeight };

    float buttonSize = 20.0f;
    Rectangle closeButtonRect = { panelRect.x + panelRect.width - buttonSize - 10, panelRect.y + 10, buttonSize, buttonSize };

    bool mouseOverClose = CheckCollisionPointRec(GetMousePosition(), closeButtonRect);
    if (mouseOverClose && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        selectedTile_ = std::nullopt;
        return;
    }

    DrawRectangleRounded(panelRect, 0.1f, 10, ColorAlpha(BLACK, 0.8f));
    DrawRectangleRoundedLines(panelRect, 0.1f, 10, WHITE);

    std::string title = "Tile (" + std::to_string(selectedTile_->x) + ", " + std::to_string(selectedTile_->y) + ")";
    DrawText(title.c_str(), panelX + 10, panelY + 10, 20, WHITE);

    DrawLine(closeButtonRect.x, closeButtonRect.y, closeButtonRect.x + closeButtonRect.width, closeButtonRect.y + closeButtonRect.height, mouseOverClose ? RED : WHITE);
    DrawLine(closeButtonRect.x, closeButtonRect.y + closeButtonRect.height, closeButtonRect.x + closeButtonRect.width, closeButtonRect.y, mouseOverClose ? RED : WHITE);

    const Tile& tile = world.getTile(selectedTile_->x, selectedTile_->y);
    const auto& resources = tile.getResources();
    int yPos = panelY + 40;

    for (int i = 0; i < static_cast<int>(ResourceType::Count); ++i) {
        ResourceType type = static_cast<ResourceType>(i);
        DrawCircle(panelX + 20, yPos + 8, 8, getResourceColor(type));
        std::string resourceInfo = getResourceName(type) + ": " + std::to_string(resources[i]);
        DrawText(resourceInfo.c_str(), panelX + 40, yPos, 16, WHITE);
        yPos += 20;
    }
}

void Renderer3D::updateCamera3D(const Camera& camera2D) {
    sf::Vector2f pos2D = camera2D.getPosition();
    float zoom = camera2D.getZoom();

    camera3D_.target.x = pos2D.x / 32.0f;
    camera3D_.target.z = pos2D.y / 32.0f;
    camera3D_.target.y = 0.0f;

    float distance = 20.0f / zoom;
    distance = std::max(5.0f, std::min(distance, 50.0f));

    camera3D_.position.x = camera3D_.target.x;
    camera3D_.position.y = distance * 0.7f;
    camera3D_.position.z = camera3D_.target.z + distance * 0.7f;
}

Vector3 Renderer3D::toVector3(float x, float y, float z) const {
    return (Vector3){ x, y, z };
}

Color Renderer3D::getResourceColor(ResourceType type) const {
    switch (type) {
        case ResourceType::Food: return BROWN;
        case ResourceType::Linemate: return LIGHTGRAY;
        case ResourceType::Deraumere: return BLUE;
        case ResourceType::Sibur: return GREEN;
        case ResourceType::Mendiane: return PINK;
        case ResourceType::Phiras: return ORANGE;
        case ResourceType::Thystame: return PURPLE;
        default: return WHITE;
    }
}

std::string Renderer3D::getResourceName(ResourceType type) const {
    switch (type) {
        case ResourceType::Food: return "Food";
        case ResourceType::Linemate: return "Linemate";
        case ResourceType::Deraumere: return "Deraumere";
        case ResourceType::Sibur: return "Sibur";
        case ResourceType::Mendiane: return "Mendiane";
        case ResourceType::Phiras: return "Phiras";
        case ResourceType::Thystame: return "Thystame";
        default: return "Unknown";
    }
}

void Renderer3D::renderShadow(const Vector3& position, float size) {
    if (!shadowsEnabled_)
        return;

    Vector3 shadowPos = position;
    shadowPos.y = 0.01f;

    shadowPos.x += lightDirection_.x * (position.y * 0.5f);
    shadowPos.z += lightDirection_.z * (position.y * 0.5f);

    DrawCube(shadowPos, size * 0.8f, 0.01f, size * 0.6f, shadowColor_);
}

}
