#pragma once
#include "Common.hpp"
#include "Message.hpp"
namespace zappy {
class MessageParser {
public:
    MessageParser();
    ~MessageParser() = default;

    std::optional<Message> parse(const std::string& rawMessage) const;
    std::vector<Message> parseBuffer(std::string& buffer) const;

private:
    using ParseFunction = std::function<std::optional<Message>(const std::vector<std::string>&)>;
    std::unordered_map<std::string, ParseFunction> handlers_;

    void registerHandlers();

    std::optional<Message> parseMapSize(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseTileContent(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseTeamName(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerNew(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerPosition(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerLevel(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerInventory(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerExpel(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerBroadcast(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseIncantationStart(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseIncantationEnd(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerFork(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerDrop(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerTake(const std::vector<std::string>& tokens) const;
    std::optional<Message> parsePlayerDeath(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseEggNew(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseEggHatch(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseEggDeath(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseTimeUnit(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseGameEnd(const std::vector<std::string>& tokens) const;
    std::optional<Message> parseServerMessage(const std::vector<std::string>& tokens) const;

    std::vector<std::string> tokenize(const std::string& message) const;
    bool validateParameterCount(const std::vector<std::string>& tokens, size_t expected) const;

    static bool isValidPlayerId(const std::string& str);
    static bool isValidCoordinate(const std::string& str);
    static bool isValidResourceCount(const std::string& str);
    static bool isValidOrientation(const std::string& str);
    static bool isValidLevel(const std::string& str);
};
}
