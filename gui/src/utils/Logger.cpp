#include "utils/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace zappy {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : minLevel_(LogLevel::DEBUG)
    , logToFile_(false)
    , logToConsole_(true)
    , showTimestamp_(true)
    , showLevel_(true)
    , maxBufferSize_(100)
    , autoFlush_(true)
{
    logBuffer_.reserve(maxBufferSize_);
}

Logger::~Logger() 
{
    flush();
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::setLevel(LogLevel level) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    minLevel_ = level;
}

void Logger::setLogToFile(bool enable, const std::string& filename) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logToFile_ = enable;
    if (enable) {
        logFilename_ = filename;
        logFile_.open(logFilename_, std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "Failed to open log file: " << logFilename_ << std::endl;
            logToFile_ = false;
        }
    }
}

void Logger::setLogToConsole(bool enable) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    logToConsole_ = enable;
}

void Logger::setShowTimestamp(bool show) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    showTimestamp_ = show;
}

void Logger::setShowLevel(bool show) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    showLevel_ = show;
}

void Logger::log(LogLevel level, const std::string& message) 
{
    if (level < minLevel_) return;
    writeLog(level, message);
}

void Logger::debug(const std::string& message) 
{
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) 
{
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) 
{
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) 
{
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) 
{
    log(LogLevel::FATAL, message);
    flush();
}

void Logger::logNetworkMessage(bool incoming, const std::string& message) 
{
    std::string direction = incoming ? "<<" : ">>";
    log(LogLevel::DEBUG, "[NET] " + direction + " " + message);
}

void Logger::logNetworkError(const std::string& error) 
{
    log(LogLevel::ERROR, "[NET] " + error);
}

void Logger::logPerformance(const std::string& metric, float value) 
{
    log(LogLevel::DEBUG, "[PERF] " + metric + ": " + std::to_string(value));
}

void Logger::logFrameTime(float frameTime) 
{
    if (frameTime > 1.0f / 30.0f) {
        log(LogLevel::WARNING, "[PERF] High frame time: " + std::to_string(frameTime * 1000.0f) + "ms");
    }
}

void Logger::flush() 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) {
        for (const auto& message : logBuffer_) {
            logFile_ << message << std::endl;
        }
        logFile_.flush();
    }
    logBuffer_.clear();
}

void Logger::writeLog(LogLevel level, const std::string& message) 
{
    std::lock_guard<std::mutex> lock(logMutex_);
    std::string formattedMessage = formatMessage(level, message);
    if (logToConsole_) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formattedMessage << std::endl;
        } else {
            std::cout << formattedMessage << std::endl;
        }
    }
    if (logToFile_) {
        if (autoFlush_) {
            if (logFile_.is_open()) {
                logFile_ << formattedMessage << std::endl;
                logFile_.flush();
            }
        } else {
            logBuffer_.push_back(formattedMessage);
            if (logBuffer_.size() >= maxBufferSize_) {
                flush();
            }
        }
    }
}

std::string Logger::formatMessage(LogLevel level, const std::string& message) 
{
    std::ostringstream oss;
    if (showTimestamp_) {
        oss << "[" << getCurrentTimestamp() << "] ";
    }
    if (showLevel_) {
        oss << "[" << levelToString(level) << "] ";
    }
    oss << message;
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) 
{
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() 
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

}
