#pragma once
#include "Common.hpp"
namespace zappy {
struct Message {
    MessageType type;
    std::vector<std::string> parameters;
    TimeStamp timestamp;

    Message() = default;
    Message(MessageType t, std::vector<std::string> params);

    bool isMapSize() const;
    bool isTileContent() const;
    bool isPlayerUpdate() const;

    template<typename T>
    std::optional<T> getParam(size_t index) const;
    std::optional<int> getIntParam(size_t index) const;
    std::optional<float> getFloatParam(size_t index) const;
    std::optional<std::string> getStringParam(size_t index) const;

    bool hasMinParams(size_t count) const;
    std::string toString() const;
};

template<>
std::optional<int> Message::getParam<int>(size_t index) const;

template<>
std::optional<float> Message::getParam<float>(size_t index) const;

template<>
std::optional<std::string> Message::getParam<std::string>(size_t index) const;
}
