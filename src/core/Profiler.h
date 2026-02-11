#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// RAII scoped timer that records duration on destruction.
class ScopedTimer
{
public:
    explicit ScopedTimer(std::string name);
    ~ScopedTimer();

    ScopedTimer(const ScopedTimer&) = delete;
    auto operator=(const ScopedTimer&) -> ScopedTimer& = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    auto operator=(ScopedTimer&&) -> ScopedTimer& = delete;

private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

/// Thread-safe performance profiler singleton.
/// Collects named timing samples and provides aggregate statistics.
class Profiler
{
public:
    static auto instance() -> Profiler&;

    // ── Manual timing ──

    void begin(std::string_view name);
    void end(std::string_view name);

    // ── Scoped timing ──

    [[nodiscard]] auto scope(std::string_view name) -> ScopedTimer;

    // ── Record a completed measurement ──

    void record(std::string_view name, double duration_ms);

    // ── Results ──

    struct TimingResult
    {
        std::string name;
        double avg_ms{0.0};
        double min_ms{0.0};
        double max_ms{0.0};
        size_t call_count{0};
    };

    [[nodiscard]] auto results() const -> std::vector<TimingResult>;
    void reset();
    void dump_to_log() const;

    // ── Memory tracking ──

    [[nodiscard]] static auto memory_usage_mb() -> double;

private:
    Profiler() = default;

    struct TimingData
    {
        std::vector<double> durations_ms;
    };

    struct PendingTimer
    {
        std::chrono::high_resolution_clock::time_point start;
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, TimingData> timings_;
    std::unordered_map<std::string, PendingTimer> pending_;
};

// Convenience macro — creates a unique variable per line
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_PROFILE_SCOPE(name)                                                                \
    auto _profiler_scope_##__LINE__ = markamp::core::Profiler::instance().scope(name)

} // namespace markamp::core
