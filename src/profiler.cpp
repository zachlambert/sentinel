#include "sentinel/profiler.h"
#include <iomanip>
#include <iostream>

namespace sentinel {

thread_local Profiler* Profiler::profiler = nullptr;

std::ostream& operator<<(std::ostream& os, const Profile& profile)
{
    if (&os == &std::cout) {
        os << std::setw(20) << std::left << "\033[1;37mPROFILE\033[0m";
        os << std::setw(40) << std::left << profile.label;
        os << std::setw(30) << std::left << profile.timestamp;
        if (profile.duration.has_value()) {
            std::stringstream ss;
            ss << "Duration: " << profile.duration.value();
            os << std::setw(30) << std::left << ss.str();
        }
    } else {
        os << "PROFILE | " << profile.label;
        os << " | " << profile.timestamp;
        if (profile.duration.has_value()) {
            os << " | Duration: " << profile.duration.value();
        }
    }
    return os;
}

void profile_point(const std::string_view& label)
{
    Profiler::profile_point(label);
}

Profiler::ScopedFrame profile_frame(const std::string_view& label)
{
    return Profiler::profile_frame(label);
}

} // namespace sentinel
