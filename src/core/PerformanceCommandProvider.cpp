// ============================================================================
// File: src/core/PerformanceCommandProvider.cpp
// Phase 30: Performance Optimization — Command palette commands
// ============================================================================

#include "PerformanceCommandProvider.h"

#include <algorithm>
#include <set>

namespace markamp::core
{

PerformanceCommandProvider::PerformanceCommandProvider()
{
    register_commands();
}

auto PerformanceCommandProvider::commands() const -> const std::vector<PerformanceCommand>&
{
    return commands_;
}

auto PerformanceCommandProvider::find_command(const std::string& command_id) const
    -> std::optional<PerformanceCommand>
{
    auto iter = std::find_if(commands_.begin(),
                             commands_.end(),
                             [&](const PerformanceCommand& cmd) { return cmd.id == command_id; });
    if (iter != commands_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto PerformanceCommandProvider::commands_in_category(const std::string& category) const
    -> std::vector<PerformanceCommand>
{
    std::vector<PerformanceCommand> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto PerformanceCommandProvider::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique_cats;
    for (const auto& cmd : commands_)
    {
        unique_cats.insert(cmd.category);
    }
    return {unique_cats.begin(), unique_cats.end()};
}

auto PerformanceCommandProvider::command_count() const -> int32_t
{
    return static_cast<int32_t>(commands_.size());
}

auto PerformanceCommandProvider::available_commands() const -> std::vector<PerformanceCommand>
{
    return commands_;
}

auto PerformanceCommandProvider::register_commands() -> void
{
    commands_ = {
        {"perf.showMetrics",
         "Performance: Show Metrics",
         "Performance",
         "",
         "Open the performance metrics output channel"},
        {"perf.toggleOverlay",
         "Performance: Toggle Debug Overlay",
         "Performance",
         "Ctrl+Shift+P",
         "Toggle the FPS/memory/frame-time debug overlay"},
        {"perf.reportStartup",
         "Performance: Report Startup Time",
         "Performance",
         "",
         "Report startup phase timings to output channel"},
        {"perf.profileFrames",
         "Performance: Profile Next 60 Frames",
         "Performance",
         "",
         "Capture frame timing for the next 60 frames"},
        {"perf.memoryReport",
         "Performance: Memory Report",
         "Performance",
         "",
         "Show per-subsystem memory usage and budget compliance"},
        {"perf.resetCounters",
         "Performance: Reset Counters",
         "Performance Diagnostics",
         "",
         "Reset all performance counters and frame statistics"},
        {"perf.toggleThrottle",
         "Performance: Toggle Adaptive Throttle",
         "Performance Diagnostics",
         "",
         "Toggle adaptive throttle between typing and idle modes"},
    };
}

} // namespace markamp::core
