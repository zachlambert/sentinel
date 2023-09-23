#pragma once

#include <array>
#include <thread>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include "cpp_utils/annotated_enum.h"


namespace sentinel {

enum class LogFlag {
    // Used for status information
    INFO,
    // Used for non-critical errors
    WARN,
    // Used for critical errors
    ERROR,
    // Used for debugging and dumping information
    DEBUG
};
std::ostream& operator<<(std::ostream& os, LogFlag flag);

struct Log {
    LogFlag flag;
    std::string label;
    std::string message;
    int64_t timestamp;
};
std::ostream& operator<<(std::ostream& os, const Log& log);

class Logger: public std::streambuf, public std::ostream {
    static thread_local Logger* logger;
public:
    using timestamp_t = int64_t;
    using timestamp_source_t = std::function<timestamp_t()>;
    using log_callback_t = std::function<void(Log&)>;

    Logger(
        const std::string& label,
        const timestamp_source_t& timestamp_source,
        const log_callback_t& log_callback
    ):
        std::ostream(this),
        timestamp_source(timestamp_source),
        log_callback(log_callback)
    {
        log.label = label;
    }
    ~Logger()
    {
        logger = nullptr;
    }

    void attach()
    {
        logger = this;
    }

protected:
    int overflow(int c) override
    {
        char ch = std::char_traits<char>::to_char_type(c);
        log.message.push_back(c);
        return c;
    }
    int sync() override
    {
        // Remove the newline at the end
        log.message.pop_back();

        log.timestamp = timestamp_source();
        log_callback(log);
        return 0;
    }

private:
    static inline std::ostream& get_log(LogFlag log_flag)
    {
        if (logger) {
            logger->log.message.clear();
            logger->log.flag = log_flag;
            return *logger;
        } else {
            std::cout << "[" << log_flag << "] ";
            return std::cout;
        }
    }

    const timestamp_source_t timestamp_source;
    const log_callback_t log_callback;
    Log log;

    friend std::ostream& log_info();
    friend std::ostream& log_warn();
    friend std::ostream& log_error();
    friend std::ostream& log_debug();
};

inline std::ostream& log_info() { return Logger::get_log(LogFlag::INFO); }
inline std::ostream& log_warn() { return Logger::get_log(LogFlag::WARN); }
inline std::ostream& log_error() { return Logger::get_log(LogFlag::ERROR); }
inline std::ostream& log_debug() { return Logger::get_log(LogFlag::DEBUG); }

} // namespace sentinel

template <>
struct cpp_utils::enum_details<sentinel::LogFlag> {
    static constexpr size_t count = 4;
    static sentinel::LogFlag get(size_t index) {
        if (index >= count) {
            throw std::out_of_range("Attempted to construct sentinel::LogFlag from an invalid index");
        }
        return (sentinel::LogFlag)index;
    }
    static std::size_t index(sentinel::LogFlag flag) {
        return (std::size_t)flag;
    }
    static const char* label(std::size_t index) {
        const char* labels[] = {
            "INFO", "WARN", "ERROR", "DEBUG"
        };
        return labels[index];
    }
};
