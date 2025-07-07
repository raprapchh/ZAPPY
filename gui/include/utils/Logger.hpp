#pragma once
#include "Common.hpp"
#include <fstream>
#include <mutex>
namespace zappy {
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};
class Logger {
public:
    static Logger& getInstance();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void setLevel(LogLevel level);
    void setLogToFile(bool enable, const std::string& filename = "zappy_gui.log");
    void setLogToConsole(bool enable);
    void setShowTimestamp(bool show);
    void setShowLevel(bool show);

    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

    template<typename... Args>
    void logf(LogLevel level, const std::string& format, Args&&... args);
    template<typename... Args>
    void debugf(const std::string& format, Args&&... args);
    template<typename... Args>
    void infof(const std::string& format, Args&&... args);
    template<typename... Args>
    void warningf(const std::string& format, Args&&... args);
    template<typename... Args>
    void errorf(const std::string& format, Args&&... args);
    template<typename... Args>
    void fatalf(const std::string& format, Args&&... args);

    void logNetworkMessage(bool incoming, const std::string& message);
    void logNetworkError(const std::string& error);
    void logPerformance(const std::string& metric, float value);
    void logFrameTime(float frameTime);

    void flush();

private:
    Logger();
    void writeLog(LogLevel level, const std::string& message);
    std::string formatMessage(LogLevel level, const std::string& message);
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();

    LogLevel minLevel_;
    bool logToFile_;
    bool logToConsole_;
    bool showTimestamp_;
    bool showLevel_;
    std::ofstream logFile_;
    std::string logFilename_;
    std::mutex logMutex_;
    std::vector<std::string> logBuffer_;
    size_t maxBufferSize_;
    bool autoFlush_;
};
template<typename... Args>
void Logger::logf(LogLevel level, const std::string& format, Args&&... args) {
    if (level < minLevel_) return;
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), format.c_str(), std::forward<Args>(args)...);
    log(level, std::string(buffer));
}
template<typename... Args>
void Logger::debugf(const std::string& format, Args&&... args) {
    logf(LogLevel::DEBUG, format, std::forward<Args>(args)...);
}
template<typename... Args>
void Logger::infof(const std::string& format, Args&&... args) {
    logf(LogLevel::INFO, format, std::forward<Args>(args)...);
}
template<typename... Args>
void Logger::warningf(const std::string& format, Args&&... args) {
    logf(LogLevel::WARNING, format, std::forward<Args>(args)...);
}
template<typename... Args>
void Logger::errorf(const std::string& format, Args&&... args) {
    logf(LogLevel::ERROR, format, std::forward<Args>(args)...);
}
template<typename... Args>
void Logger::fatalf(const std::string& format, Args&&... args) {
    logf(LogLevel::FATAL, format, std::forward<Args>(args)...);
}
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_FATAL(msg) Logger::getInstance().fatal(msg)
#define LOG_DEBUGF(fmt, ...) Logger::getInstance().debugf(fmt, __VA_ARGS__)
#define LOG_INFOF(fmt, ...) Logger::getInstance().infof(fmt, __VA_ARGS__)
#define LOG_WARNINGF(fmt, ...) Logger::getInstance().warningf(fmt, __VA_ARGS__)
#define LOG_ERRORF(fmt, ...) Logger::getInstance().errorf(fmt, __VA_ARGS__)
#define LOG_FATALF(fmt, ...) Logger::getInstance().fatalf(fmt, __VA_ARGS__)
}
