#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <optional>
#include <variant>
#include <condition_variable>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
using PlayerID = int;
using TeamID = int;
using EggID = int;
using TimeStamp = std::chrono::steady_clock::time_point;
using Duration = std::chrono::milliseconds;
namespace zappy {
    class Application;
    class Config;
    class NetworkClient;
    class World;
    class Renderer;
    class UIManager;
    class Camera;
    class Entity;
    class Player;
    class Team;
    class Egg;
    class Tile;
    enum class ResourceType : int {
        Food = 0,
        Linemate = 1,
        Deraumere = 2,
        Sibur = 3,
        Mendiane = 4,
        Phiras = 5,
        Thystame = 6,
        Count = 7
    };
    enum class Orientation : int {
        North = 1,
        East = 2,
        South = 3,
        West = 4
    };
    enum class MessageType {
        MapSize,
        TileContent,
        TeamName,
        PlayerNew,
        PlayerPosition,
        PlayerLevel,
        PlayerInventory,
        PlayerExpel,
        PlayerBroadcast,
        IncantationStart,
        IncantationEnd,
        PlayerFork,
        PlayerDrop,
        PlayerTake,
        PlayerDeath,
        EggNew,
        EggHatch,
        EggDeath,
        TimeUnit,
        GameEnd,
        ServerMessage,
        Unknown
    };
}
namespace zappy::constants {
    constexpr int RESOURCE_COUNT = 7;
    constexpr int MAX_LEVEL = 8;
    constexpr float TILE_SIZE = 32.0f;
    constexpr int DEFAULT_WINDOW_WIDTH = 1280;
    constexpr int DEFAULT_WINDOW_HEIGHT = 720;
}
