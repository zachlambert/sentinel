#include "sentinel/logger.h"


namespace sentinel {

thread_local Logger* Logger::logger = nullptr;

std::ostream& operator<<(std::ostream& os, LogFlag flag) {
    if (&os == &std::cout) {
        const char* colors[cpp_utils::enum_count<LogFlag>] = { "32", "33", "31", "36" };
        os << "\033[1;" << colors[(size_t)flag] << "m" << cpp_utils::enum_label(flag) << "\033[0m";
    } else {
        os << cpp_utils::enum_label(flag);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Log& log) {
    os << log.flag << " | " << log.label << " | " << log.message;
    return os;
}

} // namespace sentinel
