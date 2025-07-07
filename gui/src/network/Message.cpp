#include "network/Message.hpp"
#include <sstream>
#include <iomanip>

namespace zappy {

Message::Message(MessageType t, std::vector<std::string> params)
    : type(t), parameters(std::move(params)), timestamp(std::chrono::steady_clock::now())     
{
}

bool Message::isMapSize() const 
{
    return type == MessageType::MapSize;
}

bool Message::isTileContent() const 
{
    return type == MessageType::TileContent;
}

bool Message::isPlayerUpdate() const 
{
    return type == MessageType::PlayerNew ||
           type == MessageType::PlayerPosition ||
           type == MessageType::PlayerLevel ||
           type == MessageType::PlayerInventory;
}

bool Message::hasMinParams(size_t count) const 
{
    return parameters.size() >= count;
}

std::optional<int> Message::getIntParam(size_t index) const
{
    if (index >= parameters.size()) {
        return std::nullopt;
    }
    try {
        return std::stoi(parameters[index]);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<float> Message::getFloatParam(size_t index) const 
{
    if (index >= parameters.size()) {
        return std::nullopt;
    }
    try {
        return std::stof(parameters[index]);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<std::string> Message::getStringParam(size_t index) const 
{
    if (index >= parameters.size()) {
        return std::nullopt;
    }
    return parameters[index];
}

std::string Message::toString() const 
{
    std::ostringstream oss;
    const char* typeStr = "UNKNOWN";
    switch (type) {
        case MessageType::MapSize: typeStr = "MSZ"; break;
        case MessageType::TileContent: typeStr = "BCT"; break;
        case MessageType::TeamName: typeStr = "TNA"; break;
        case MessageType::PlayerNew: typeStr = "PNW"; break;
        case MessageType::PlayerPosition: typeStr = "PPO"; break;
        case MessageType::PlayerLevel: typeStr = "PLV"; break;
        case MessageType::PlayerInventory: typeStr = "PIN"; break;
        case MessageType::PlayerExpel: typeStr = "PEX"; break;
        case MessageType::PlayerBroadcast: typeStr = "PBC"; break;
        case MessageType::IncantationStart: typeStr = "PIC"; break;
        case MessageType::IncantationEnd: typeStr = "PIE"; break;
        case MessageType::PlayerFork: typeStr = "PFK"; break;
        case MessageType::PlayerDrop: typeStr = "PDR"; break;
        case MessageType::PlayerTake: typeStr = "PGT"; break;
        case MessageType::PlayerDeath: typeStr = "PDI"; break;
        case MessageType::EggNew: typeStr = "ENW"; break;
        case MessageType::EggHatch: typeStr = "EBO"; break;
        case MessageType::EggDeath: typeStr = "EDI"; break;
        case MessageType::TimeUnit: typeStr = "SGT"; break;
        case MessageType::GameEnd: typeStr = "SEG"; break;
        case MessageType::ServerMessage: typeStr = "SMG"; break;
        default: break;
    }
    oss << typeStr;
    for (const auto& param : parameters) {
        oss << " " << param;
    }
    return oss.str();
}

template<>
std::optional<int> Message::getParam<int>(size_t index) const 
{
    return getIntParam(index);
}

template<>
std::optional<float> Message::getParam<float>(size_t index) const 
{
    return getFloatParam(index);
}

template<>
std::optional<std::string> Message::getParam<std::string>(size_t index) const 
{
    return getStringParam(index);
}

}
