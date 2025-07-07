#pragma once
#include "Common.hpp"
#include "Message.hpp"
#include "ThreadSafeQueue.hpp"
namespace zappy {
class NetworkClient {
public:
    enum class State {
        Disconnected,
        Connecting,
        Connected,
        Error
    };

    explicit NetworkClient(const Config& config);
    ~NetworkClient();

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;
    NetworkClient(NetworkClient&&) = default;
    NetworkClient& operator=(NetworkClient&&) = default;

    bool connect();
    void disconnect();
    void sendCommand(const std::string& command);
    std::optional<Message> getNextMessage();
    bool hasMessages() const;

    State getState() const;
    bool isConnected() const;
    size_t getBytesSent() const;
    size_t getBytesReceived() const;
    Duration getLatency() const;

private:
    void networkThreadFunction();
    void processIncomingData();
    void sendPendingCommands();
    bool performHandshake();
    void handleSocketError();
    void cleanup();

    const Config& config_;
    std::atomic<State> state_{State::Disconnected};
    sf::TcpSocket socket_;
    std::thread networkThread_;
    std::atomic<bool> shouldStop_{false};
    ThreadSafeQueue<std::string> outgoingCommands_;
    ThreadSafeQueue<Message> incomingMessages_;
    std::string receiveBuffer_;
    std::atomic<size_t> bytesSent_{0};
    std::atomic<size_t> bytesReceived_{0};
    mutable std::mutex latencyMutex_;
    Duration lastLatency_{0};
    mutable std::mutex socketMutex_;
    TimeStamp lastActivity_;
};
}
