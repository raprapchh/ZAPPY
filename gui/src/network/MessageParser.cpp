#include "network/MessageParser.hpp"
#include <sstream>
#include <algorithm>
#include <regex>

namespace zappy {

MessageParser::MessageParser()
{
    registerHandlers();
}

void MessageParser::registerHandlers() 
{
    handlers_["msz"] = [this](const std::vector<std::string>& tokens) { return parseMapSize(tokens); };
    handlers_["bct"] = [this](const std::vector<std::string>& tokens) { return parseTileContent(tokens); };
    handlers_["tna"] = [this](const std::vector<std::string>& tokens) { return parseTeamName(tokens); };
    handlers_["pnw"] = [this](const std::vector<std::string>& tokens) { return parsePlayerNew(tokens); };
    handlers_["ppo"] = [this](const std::vector<std::string>& tokens) { return parsePlayerPosition(tokens); };
    handlers_["plv"] = [this](const std::vector<std::string>& tokens) { return parsePlayerLevel(tokens); };
    handlers_["pin"] = [this](const std::vector<std::string>& tokens) { return parsePlayerInventory(tokens); };
    handlers_["pex"] = [this](const std::vector<std::string>& tokens) { return parsePlayerExpel(tokens); };
    handlers_["pbc"] = [this](const std::vector<std::string>& tokens) { return parsePlayerBroadcast(tokens); };
    handlers_["pic"] = [this](const std::vector<std::string>& tokens) { return parseIncantationStart(tokens); };
    handlers_["pie"] = [this](const std::vector<std::string>& tokens) { return parseIncantationEnd(tokens); };
    handlers_["pfk"] = [this](const std::vector<std::string>& tokens) { return parsePlayerFork(tokens); };
    handlers_["pdr"] = [this](const std::vector<std::string>& tokens) { return parsePlayerDrop(tokens); };
    handlers_["pgt"] = [this](const std::vector<std::string>& tokens) { return parsePlayerTake(tokens); };
    handlers_["pdi"] = [this](const std::vector<std::string>& tokens) { return parsePlayerDeath(tokens); };
    handlers_["enw"] = [this](const std::vector<std::string>& tokens) { return parseEggNew(tokens); };
    handlers_["ebo"] = [this](const std::vector<std::string>& tokens) { return parseEggHatch(tokens); };
    handlers_["edi"] = [this](const std::vector<std::string>& tokens) { return parseEggDeath(tokens); };
    handlers_["sgt"] = [this](const std::vector<std::string>& tokens) { return parseTimeUnit(tokens); };
    handlers_["seg"] = [this](const std::vector<std::string>& tokens) { return parseGameEnd(tokens); };
    handlers_["smg"] = [this](const std::vector<std::string>& tokens) { return parseServerMessage(tokens); };
}

std::optional<Message> MessageParser::parse(const std::string& rawMessage) const 
{
    if (rawMessage.empty()) {
        return std::nullopt;
    }

    auto tokens = tokenize(rawMessage);
    if (tokens.empty()) {
        return std::nullopt;
    }

    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    auto it = handlers_.find(command);
    if (it == handlers_.end()) {
        return Message{MessageType::Unknown, std::move(tokens)};
    }

    return it->second(tokens);
}

std::vector<Message> MessageParser::parseBuffer(std::string& buffer) const 
{
    std::vector<Message> messages;
    size_t pos = 0;
    while ((pos = buffer.find('\n')) != std::string::npos) {
        std::string line = buffer.substr(0, pos);
        buffer.erase(0, pos + 1);
        if (line.empty()) {
            continue;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto message = parse(line);
        if (message) {
            messages.emplace_back(std::move(*message));
        }
    }
    return messages;
}

std::vector<std::string> MessageParser::tokenize(const std::string& message) const 
{
    std::vector<std::string> tokens;
    std::istringstream iss(message);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool MessageParser::validateParameterCount(const std::vector<std::string>& tokens, size_t expected) const 
{
    return tokens.size() >= expected;
}

std::optional<Message> MessageParser::parseMapSize(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 3)) {
        return std::nullopt;
    }
    if (!isValidCoordinate(tokens[1]) || !isValidCoordinate(tokens[2])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1], tokens[2]};
    return Message{MessageType::MapSize, std::move(params)};
}

std::optional<Message> MessageParser::parseTileContent(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 10)) {
        return std::nullopt;
    }
    if (!isValidCoordinate(tokens[1]) || !isValidCoordinate(tokens[2])) {
        return std::nullopt;
    }
    for (size_t i = 3; i < 10; ++i) {
        if (!isValidResourceCount(tokens[i])) {
            return std::nullopt;
        }
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::TileContent, std::move(params)};
}

std::optional<Message> MessageParser::parseTeamName(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::TeamName, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerNew(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 6)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1]) || !isValidCoordinate(tokens[2]) ||
        !isValidCoordinate(tokens[3]) || !isValidOrientation(tokens[4]) ||
        !isValidLevel(tokens[5])) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerNew, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerPosition(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 5)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1]) || !isValidCoordinate(tokens[2]) ||
        !isValidCoordinate(tokens[3]) || !isValidOrientation(tokens[4])) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerPosition, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerLevel(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 3)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1]) || !isValidLevel(tokens[2])) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerLevel, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerInventory(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 10)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1]) || !isValidCoordinate(tokens[2]) ||
        !isValidCoordinate(tokens[3])) {
        return std::nullopt;
    }
    for (size_t i = 4; i < 11; ++i) {
        if (!isValidResourceCount(tokens[i])) {
            return std::nullopt;
        }
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerInventory, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerExpel(const std::vector<std::string>& tokens) const
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::PlayerExpel, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerBroadcast(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 3)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::string message;
    for (size_t i = 2; i < tokens.size(); ++i) {
        if (i > 2) message += " ";
        message += tokens[i];
    }
    std::vector<std::string> params = {tokens[1], message};
    return Message{MessageType::PlayerBroadcast, std::move(params)};
}

std::optional<Message> MessageParser::parseIncantationStart(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 4)) {
        return std::nullopt;
    }
    if (!isValidCoordinate(tokens[1]) || !isValidCoordinate(tokens[2]) ||
        !isValidLevel(tokens[3])) {
        return std::nullopt;
    }
    for (size_t i = 4; i < tokens.size(); ++i) {
        if (!isValidPlayerId(tokens[i])) {
            return std::nullopt;
        }
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::IncantationStart, std::move(params)};
}

std::optional<Message> MessageParser::parseIncantationEnd(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 4)) {
        return std::nullopt;
    }
    if (!isValidCoordinate(tokens[1]) || !isValidCoordinate(tokens[2])) {
        return std::nullopt;
    }
    if (tokens[3] != "0" && tokens[3] != "1") {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::IncantationEnd, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerFork(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::PlayerFork, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerDrop(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 3)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    try {
        int resourceType = std::stoi(tokens[2]);
        if (resourceType < 0 || resourceType > 6) {
            return std::nullopt;
        }
    } catch (const std::exception&) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerDrop, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerTake(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 3)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    try {
        int resourceType = std::stoi(tokens[2]);
        if (resourceType < 0 || resourceType > 6) {
            return std::nullopt;
        }
    } catch (const std::exception&) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::PlayerTake, std::move(params)};
}

std::optional<Message> MessageParser::parsePlayerDeath(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::PlayerDeath, std::move(params)};
}

std::optional<Message> MessageParser::parseEggNew(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 5)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1]) || !isValidPlayerId(tokens[2]) ||
        !isValidCoordinate(tokens[3]) || !isValidCoordinate(tokens[4])) {
        return std::nullopt;
    }
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    return Message{MessageType::EggNew, std::move(params)};
}

std::optional<Message> MessageParser::parseEggHatch(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::EggHatch, std::move(params)};
}

std::optional<Message> MessageParser::parseEggDeath(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    if (!isValidPlayerId(tokens[1])) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::EggDeath, std::move(params)};
}

std::optional<Message> MessageParser::parseTimeUnit(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    try {
        std::stoi(tokens[1]);
    } catch (const std::exception&) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::TimeUnit, std::move(params)};
}

std::optional<Message> MessageParser::parseGameEnd(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    std::vector<std::string> params = {tokens[1]};
    return Message{MessageType::GameEnd, std::move(params)};
}

std::optional<Message> MessageParser::parseServerMessage(const std::vector<std::string>& tokens) const 
{
    if (!validateParameterCount(tokens, 2)) {
        return std::nullopt;
    }
    std::string message;
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (i > 1) message += " ";
        message += tokens[i];
    }
    std::vector<std::string> params = {message};
    return Message{MessageType::ServerMessage, std::move(params)};
}

bool MessageParser::isValidPlayerId(const std::string& str) 
{
    if (str.empty() || str[0] != '#') {
        return false;
    }
    try {
        int id = std::stoi(str.substr(1));
        return id >= 0;
    } catch (const std::exception&) {
        return false;
    }
}

bool MessageParser::isValidCoordinate(const std::string& str) 
{
    try {
        int coord = std::stoi(str);
        return coord >= 0;
    } catch (const std::exception&) {
        return false;
    }
}

bool MessageParser::isValidResourceCount(const std::string& str) 
{
    try {
        int count = std::stoi(str);
        return count >= 0;
    } catch (const std::exception&) {
        return false;
    }
}

bool MessageParser::isValidOrientation(const std::string& str) 
{
    try {
        int orientation = std::stoi(str);
        return orientation >= 1 && orientation <= 4;
    } catch (const std::exception&) {
        return false;
    }
}

bool MessageParser::isValidLevel(const std::string& str) 
{
    try {
        int level = std::stoi(str);
        return level >= 1 && level <= 8;
    } catch (const std::exception&) {
        return false;
    }
}

}
