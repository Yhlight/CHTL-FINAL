#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace CHTL {

// 日志级别
enum class LogLevel {
    DEBUG,      // 调试信息
    INFO,       // 一般信息
    WARNING,    // 警告
    ERROR,      // 错误
    FATAL       // 致命错误
};

// 简单的日志类
class Logger {
private:
    LogLevel minLevel_;
    std::ostream& output_;
    bool showTimestamp_;

public:
    Logger(LogLevel minLevel = LogLevel::INFO, 
           std::ostream& output = std::cerr,
           bool showTimestamp = false)
        : minLevel_(minLevel)
        , output_(output)
        , showTimestamp_(showTimestamp)
    {}

    void setLevel(LogLevel level) {
        minLevel_ = level;
    }

    void setTimestamp(bool show) {
        showTimestamp_ = show;
    }

    template<typename... Args>
    void debug(Args&&... args) {
        log(LogLevel::DEBUG, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(Args&&... args) {
        log(LogLevel::INFO, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(Args&&... args) {
        log(LogLevel::WARNING, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(Args&&... args) {
        log(LogLevel::ERROR, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void fatal(Args&&... args) {
        log(LogLevel::FATAL, std::forward<Args>(args)...);
    }

private:
    template<typename... Args>
    void log(LogLevel level, Args&&... args) {
        if (level < minLevel_) {
            return;
        }

        // 时间戳
        if (showTimestamp_) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            output_ << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S] ");
        }

        // 级别
        output_ << "[" << levelToString(level) << "] ";

        // 消息
        std::ostringstream oss;
        (oss << ... << args);
        output_ << oss.str() << "\n";
    }

    static const char* levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO ";
            case LogLevel::WARNING: return "WARN ";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::FATAL:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }
};

// 全局日志实例
inline Logger& getLogger() {
    static Logger logger;
    return logger;
}

} // namespace CHTL
