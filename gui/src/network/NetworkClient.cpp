#include "network/NetworkClient.hpp"
#include "network/MessageParser.hpp"
#include "Config.hpp"
#include <iostream>
#include <thread>

namespace zappy {

NetworkClient::NetworkClient(const Config& config)
    : config_(config), lastActivity_(std::chrono::steady_clock::now()) 
{
}

NetworkClient::~NetworkClient() 
{
    disconnect();
}

bool NetworkClient::connect() 
{
    std::lock_guard<std::mutex> lock(socketMutex_);
    if (state_.load() == State::Connected) {
        return true;
    }
    state_ = State::Connecting;
    sf::Time timeout = sf::milliseconds(config_.networkTimeout.count());
    if (socket_.connect(config_.serverHost, config_.serverPort, timeout) != sf::Socket::Done) {
        std::cerr << "Failed to connect to " << config_.serverHost << ":" << config_.serverPort << std::endl;
        state_ = State::Error;
        return false;
    }
    socket_.setBlocking(false);
    if (!performHandshake()) {
        socket_.disconnect();
        state_ = State::Error;
        return false;
    }
    state_ = State::Connected;
    shouldStop_ = false;
    networkThread_ = std::thread(&NetworkClient::networkThreadFunction, this);
    return true;
}

void NetworkClient::disconnect() 
{
    if (state_.load() == State::Disconnected) {
        return;
    }
    shouldStop_ = true;
    if (networkThread_.joinable()) {
        networkThread_.join();
    }
    {
        std::lock_guard<std::mutex> lock(socketMutex_);
        socket_.disconnect();
    }
    state_ = State::Disconnected;
    outgoingCommands_.clear();
    incomingMessages_.clear();
    receiveBuffer_.clear();
    std::cout << "Disconnected from server" << std::endl;
}

void NetworkClient::sendCommand(const std::string& command) 
{
    if (state_.load() != State::Connected) {
        return;
    }
    std::string cmdWithNewline = command;
    if (!cmdWithNewline.empty() && cmdWithNewline.back() != '\n') {
        cmdWithNewline += '\n';
    }
    outgoingCommands_.push(cmdWithNewline);
}

std::optional<Message> NetworkClient::getNextMessage() 
{
    return incomingMessages_.tryPop();
}

bool NetworkClient::hasMessages() const 
{
    return !incomingMessages_.empty();
}

NetworkClient::State NetworkClient::getState() const 
{
    return state_.load();
}

bool NetworkClient::isConnected() const 
{
    return state_.load() == State::Connected;
}

size_t NetworkClient::getBytesSent() const
{
    return bytesSent_.load();
}

size_t NetworkClient::getBytesReceived() const 
{
    return bytesReceived_.load();
}

Duration NetworkClient::getLatency() const 
{
    std::lock_guard<std::mutex> lock(latencyMutex_);
    return lastLatency_;
}

void NetworkClient::networkThreadFunction() 
{
    MessageParser parser;
    while (!shouldStop_.load()) {
        try {
            processIncomingData();
            sendPendingCommands();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (const std::exception& e) {
            std::cerr << "Network thread error: " << e.what() << std::endl;
            handleSocketError();
            break;
        }
    }
    cleanup();
}

void NetworkClient::processIncomingData() 
{
    char buffer[4096];
    size_t received;
    {
        std::lock_guard<std::mutex> lock(socketMutex_);
        if (socket_.receive(buffer, sizeof(buffer), received) != sf::Socket::Done) {
            return;
        }
    }
    if (received == 0) {
        return;
    }
    lastActivity_ = std::chrono::steady_clock::now();
    bytesReceived_ += received;
    receiveBuffer_.append(buffer, received);
    MessageParser parser;
    auto messages = parser.parseBuffer(receiveBuffer_);
    for (auto& message : messages) {
        incomingMessages_.push(std::move(message));
    }
}

void NetworkClient::sendPendingCommands() 
{
    std::string command;
    while (outgoingCommands_.tryPop(command)) {
        {
            std::lock_guard<std::mutex> lock(socketMutex_);
            size_t sent = 0;
            if (socket_.send(command.c_str(), command.length(), sent) != sf::Socket::Done) {
                std::cerr << "Failed to send command: " << command << std::endl;
                handleSocketError();
                return;
            }
        }
        bytesSent_ += command.length();
        lastActivity_ = std::chrono::steady_clock::now();
    }
}

bool NetworkClient::performHandshake() 
{
    char buffer[1024];
    size_t received;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(config_.networkTimeout.count())) {
        if (socket_.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
            std::string welcome(buffer, received);
            if (welcome.find("WELCOME") != std::string::npos) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::string graphic = "GRAPHIC\n";
    size_t sent;
    if (socket_.send(graphic.c_str(), graphic.length(), sent) != sf::Socket::Done) {
        std::cerr << "Failed to send GRAPHIC command" << std::endl;
        return false;
    }
    bytesSent_ += sent;
    sendCommand("msz");
    sendCommand("mct");
    sendCommand("tna");
    sendCommand("sgt");
    return true;
}

void NetworkClient::handleSocketError() 
{
    state_ = State::Error;
    std::cerr << "Socket error detected" << std::endl;
}

void NetworkClient::cleanup() 
{
    std::lock_guard<std::mutex> lock(socketMutex_);
    socket_.disconnect();
}

}

