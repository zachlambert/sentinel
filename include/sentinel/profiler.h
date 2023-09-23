#include <sstream>
#include <optional>
#include <functional>
#include <thread>


namespace sentinel {

struct Profile {
    std::string label;
    std::int64_t timestamp;
    std::optional<std::int64_t> duration;
    std::thread::id thread_id;
    size_t stack_depth;
};
std::ostream& operator<<(std::ostream& os, const Profile& profile);

class Profiler {
    static thread_local Profiler* profiler;
public:
    using timestamp_t = std::int64_t;
    using timestamp_source_t = std::function<timestamp_t()>;
    using profile_callback_t = std::function<void(const Profile&)>;

    Profiler(
        const std::string& label,
        const timestamp_source_t& timestamp_source,
        const profile_callback_t& profile_callback
    ):
        label(label),
        timestamp_source(timestamp_source),
        profile_callback(profile_callback),
        stack_depth(0)
    {}
    ~Profiler()
    {
        profiler = nullptr;
    }

    void attach()
    {
        profiler = this;
    }

    class ScopedFrame {
    public:
        ~ScopedFrame()
        {
            if (!parent) return;

            Profile profile;
            profile.label = parent->label;
            profile.timestamp = start_timestamp;
            profile.duration = parent->timestamp_source() - start_timestamp;
            profile.thread_id = std::this_thread::get_id();
            profile.stack_depth = parent->stack_depth;
            parent->stack_depth--;
            parent->profile_callback(profile);
        }
        ScopedFrame(ScopedFrame&& other):
            parent(other.parent),
            start_timestamp(other.start_timestamp)
        {
            other.parent = nullptr;
        }
    private:
        ScopedFrame(Profiler* parent, const std::string_view& label):
            parent(parent)
        {
            if (!parent) return;

            parent->stack_depth++;
            start_timestamp = parent->timestamp_source();
        }
        Profiler* parent;
        std::int64_t start_timestamp;
        friend class Profiler;
    };

private:
    static void profile_point(const std::string_view& label)
    {
        if (!profiler) return;

        Profile profile;
        profile.timestamp = profiler->timestamp_source();
        profile.duration = std::nullopt;
        profile.label = profiler->label;
        profile.thread_id = std::this_thread::get_id();
        profile.stack_depth = profiler->stack_depth;
        profiler->profile_callback(profile);
    }

    [[nodiscard]] static ScopedFrame profile_frame(const std::string_view& label)
    {
        return ScopedFrame(profiler, label);
    }

    const std::string label;
    const timestamp_source_t timestamp_source;
    const profile_callback_t profile_callback;
    size_t stack_depth;

    friend void profile_point(const std::string_view& label);
    friend ScopedFrame profile_frame(const std::string_view& label);
};

void profile_point(const std::string_view& label);
[[nodiscard]] Profiler::ScopedFrame profile_frame(const std::string_view& label);

} // namespace sentinel
