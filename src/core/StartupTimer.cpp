#include "StartupTimer.h"

#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

namespace markamp::core
{

StartupTimer::StartupTimer()
    : start_(Clock::now())
{
}

void StartupTimer::checkpoint(std::string name)
{
    checkpoints_.push_back(Checkpoint{std::move(name), Clock::now()});
}

auto StartupTimer::elapsed_ms() const -> double
{
    return std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
}

auto StartupTimer::total_ms() const -> double
{
    if (checkpoints_.empty())
    {
        return 0.0;
    }
    return std::chrono::duration<double, std::milli>(checkpoints_.back().timestamp - start_)
        .count();
}

auto StartupTimer::checkpoint_count() const -> std::size_t
{
    return checkpoints_.size();
}

auto StartupTimer::to_json() const -> std::string
{
    nlohmann::json report;
    report["total_ms"] = total_ms();
    report["checkpoint_count"] = checkpoints_.size();

    nlohmann::json phases = nlohmann::json::array();
    TimePoint prev = start_;

    for (const auto& checkpoint : checkpoints_)
    {
        nlohmann::json phase;
        phase["name"] = checkpoint.name;
        phase["start_offset_ms"] =
            std::chrono::duration<double, std::milli>(checkpoint.timestamp - start_).count();
        phase["duration_ms"] =
            std::chrono::duration<double, std::milli>(checkpoint.timestamp - prev).count();
        phases.push_back(std::move(phase));
        prev = checkpoint.timestamp;
    }

    report["phases"] = std::move(phases);
    return report.dump(2);
}

void StartupTimer::dump_to_log() const
{
    spdlog::info("=== Startup Timing Report ({} checkpoints, {:.1f}ms total) ===",
                 checkpoints_.size(),
                 total_ms());

    TimePoint prev = start_;
    for (const auto& checkpoint : checkpoints_)
    {
        auto duration_ms =
            std::chrono::duration<double, std::milli>(checkpoint.timestamp - prev).count();
        auto offset_ms =
            std::chrono::duration<double, std::milli>(checkpoint.timestamp - start_).count();
        spdlog::info("  [{:7.1f}ms] {:30s}  (+{:.1f}ms)", offset_ms, checkpoint.name, duration_ms);
        prev = checkpoint.timestamp;
    }

    spdlog::info("=== End Startup Timing ===");
}

} // namespace markamp::core
