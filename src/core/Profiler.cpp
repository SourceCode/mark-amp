#include "Profiler.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <spdlog/spdlog.h>

#ifdef __APPLE__
#include <mach/mach.h>
#elif defined(__linux__)
#include <fstream>
#include <string>
#elif defined(_WIN32)
#include <psapi.h>
#include <windows.h>
#endif

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// ScopedTimer
// ═══════════════════════════════════════════════════════

ScopedTimer::ScopedTimer(std::string name)
    : name_(std::move(name))
    , start_(std::chrono::high_resolution_clock::now())
{
}

ScopedTimer::~ScopedTimer()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration<double, std::milli>(end - start_).count();
    // R20 Fix 16: Clamp negative durations (possible with clock adjustments)
    if (duration_ms < 0.0)
    {
        duration_ms = 0.0;
    }
    Profiler::instance().record(name_, duration_ms);
}

// ═══════════════════════════════════════════════════════
// Profiler
// ═══════════════════════════════════════════════════════

auto Profiler::instance() -> Profiler&
{
    static Profiler profiler;
    return profiler;
}

void Profiler::begin(std::string_view name)
{
    const std::lock_guard lock(mutex_);
    pending_[std::string(name)] = PendingTimer{std::chrono::high_resolution_clock::now()};
}

void Profiler::end(std::string_view name)
{
    auto end_time = std::chrono::high_resolution_clock::now();
    const std::lock_guard lock(mutex_);

    auto key = std::string(name);
    auto iter = pending_.find(key);
    if (iter == pending_.end())
    {
        return; // no matching begin()
    }

    auto duration_ms =
        std::chrono::duration<double, std::milli>(end_time - iter->second.start).count();
    // R20 Fix 37: Cap per-metric history (same limit as record())
    auto& data = timings_[key];
    constexpr std::size_t kMaxEntries = 10'000;
    if (data.durations_ms.size() >= kMaxEntries)
    {
        data.durations_ms.erase(data.durations_ms.begin());
    }
    data.durations_ms.push_back(duration_ms);
    pending_.erase(iter);
}

auto Profiler::scope(std::string_view name) -> ScopedTimer
{
    return ScopedTimer(std::string(name));
}

void Profiler::record(std::string_view name, double duration_ms)
{
    const std::lock_guard lock(mutex_);
    auto& data = timings_[std::string(name)];
    // R20 Fix 17: Cap per-metric history to prevent unbounded memory growth
    constexpr std::size_t kMaxEntries = 10'000;
    if (data.durations_ms.size() >= kMaxEntries)
    {
        data.durations_ms.erase(data.durations_ms.begin());
    }
    data.durations_ms.push_back(duration_ms);
}

auto Profiler::results() const -> std::vector<TimingResult>
{
    const std::lock_guard lock(mutex_);
    std::vector<TimingResult> out;
    out.reserve(timings_.size());

    for (const auto& [name, data] : timings_)
    {
        if (data.durations_ms.empty())
        {
            continue;
        }

        TimingResult result;
        result.name = name;
        result.call_count = data.durations_ms.size();

        const double kSumMs =
            std::accumulate(data.durations_ms.begin(), data.durations_ms.end(), 0.0);
        result.avg_ms = kSumMs / static_cast<double>(result.call_count);

        const auto [min_it, max_it] =
            std::minmax_element(data.durations_ms.begin(), data.durations_ms.end());
        result.min_ms = *min_it;
        result.max_ms = *max_it;

        out.push_back(std::move(result));
    }

    // Sort by name for deterministic output
    std::sort(out.begin(),
              out.end(),
              [](const TimingResult& lhs, const TimingResult& rhs) { return lhs.name < rhs.name; });

    return out;
}

void Profiler::reset()
{
    std::lock_guard lock(mutex_);
    timings_.clear();
    pending_.clear();
}

void Profiler::dump_to_log() const
{
    auto timing_results = results();
    spdlog::info("=== Performance Profile ({} sections) ===", timing_results.size());

    for (const auto& result : timing_results)
    {
        spdlog::info("  {}: avg={:.2f}ms  min={:.2f}ms  max={:.2f}ms  calls={}",
                     result.name,
                     result.avg_ms,
                     result.min_ms,
                     result.max_ms,
                     result.call_count);
    }

    spdlog::info("  Memory usage: {:.1f} MB", memory_usage_mb());
    spdlog::info("=== End Profile ===");
}

auto Profiler::memory_usage_mb() -> double
{
#ifdef __APPLE__
    mach_task_basic_info_data_t info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    if (task_info(
            mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &count) ==
        KERN_SUCCESS)
    {
        return static_cast<double>(info.resident_size) / (1024.0 * 1024.0);
    }
    return 0.0;
#elif defined(__linux__)
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line))
    {
        if (line.starts_with("VmRSS:"))
        {
            // Format: "VmRSS:    12345 kB"
            auto pos = line.find_first_of("0123456789");
            if (pos != std::string::npos)
            {
                // R20 Fix 12: Wrap std::stol — /proc/self/status could be corrupt
                try
                {
                    auto kb = std::stol(line.substr(pos));
                    return static_cast<double>(kb) / 1024.0;
                }
                catch (const std::exception&)
                {
                    return 0.0;
                }
            }
        }
    }
    return 0.0;
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS pmc{};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return static_cast<double>(pmc.WorkingSetSize) / (1024.0 * 1024.0);
    }
    return 0.0;
#else
    return 0.0;
#endif
}

// ═══════════════════════════════════════════════════════
// BudgetGuard implementation
// ═══════════════════════════════════════════════════════

BudgetGuard::BudgetGuard(std::string_view name, std::chrono::microseconds budget)
    : name_(name)
    , budget_(budget)
    , start_(std::chrono::high_resolution_clock::now())
{
}

BudgetGuard::~BudgetGuard()
{
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start_);
    if (elapsed > budget_)
    {
        spdlog::warn("BudgetGuard '{}' exceeded: {:.1f}us actual vs {:.1f}us budget",
                     name_,
                     static_cast<double>(elapsed.count()),
                     static_cast<double>(budget_.count()));
    }
}

} // namespace markamp::core
