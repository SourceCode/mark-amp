#pragma once

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

namespace markamp::core
{

/// Captures fine-grained timing checkpoints during application startup.
/// After all phases complete, produces a structured JSON report.
///
/// Usage:
///   StartupTimer timer;
///   timer.checkpoint("config_loaded");
///   // ... next phase ...
///   timer.checkpoint("theme_initialized");
///   auto json = timer.to_json();  // structured report
class StartupTimer
{
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    /// Constructs a StartupTimer, recording the creation time as t₀.
    StartupTimer();

    /// Record a named checkpoint at the current time.
    void checkpoint(std::string name);

    /// Total elapsed time from construction to now.
    [[nodiscard]] auto elapsed_ms() const -> double;

    /// Total elapsed time from construction to the last recorded checkpoint.
    [[nodiscard]] auto total_ms() const -> double;

    /// Number of checkpoints recorded.
    [[nodiscard]] auto checkpoint_count() const -> std::size_t;

    /// Produce a JSON string summarizing all timing phases.
    /// Each phase shows: name, start_offset_ms, duration_ms.
    [[nodiscard]] auto to_json() const -> std::string;

    /// Dump the startup report to the spdlog logger.
    void dump_to_log() const;

private:
    struct Checkpoint
    {
        std::string name;
        TimePoint timestamp;
    };

    TimePoint start_;
    std::vector<Checkpoint> checkpoints_;
};

} // namespace markamp::core
