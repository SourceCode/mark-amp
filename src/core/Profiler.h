#pragma once

#include "ProfilerIds.h"
#include "RingBuffer.h"

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
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

// Forward declaration for ScopedIdTimer
class Profiler;

/// Thread-safe performance profiler singleton.
/// Collects named timing samples and provides aggregate statistics.
class Profiler
{
public:
    /// Default constructor — allows injection of standalone instances.
    /// For shared global profiling, use instance().
    Profiler() = default;

    /// Global singleton accessor (convenience, used by MARKAMP_PROFILE_SCOPE macro).
    static auto instance() -> Profiler&;

    // ── Manual timing ──

    void begin(std::string_view name);
    void end(std::string_view name);

    // ── Scoped timing ──

    [[nodiscard]] auto scope(std::string_view name) -> ScopedTimer;

    // ── Record a completed measurement ──

    void record(std::string_view name, double duration_ms);

    /// Record a timing by ProfileId — zero allocation, O(1) array index.
    /// Phase 16 fast path.
    void record(ProfileId profile_id, double duration_ms);

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

    /// Phase 16: Fixed-size ring buffer array indexed by ProfileId.
    /// Zero allocation on the recording hot path.
    static constexpr std::size_t kRingBufferCapacity = 1024;
    struct FixedTimingData
    {
        RingBuffer<double, kRingBufferCapacity> samples;
    };
    std::array<FixedTimingData, kProfileIdCount> fixed_timings_{};
};

/// RAII scoped timer for ProfileId-based timing (Phase 16 fast path).
/// Records via Profiler::record(ProfileId, double) — zero allocation.
class ScopedIdTimer
{
public:
    explicit ScopedIdTimer(ProfileId profile_id)
        : id_(profile_id)
        , start_(std::chrono::high_resolution_clock::now())
    {
    }

    ~ScopedIdTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration<double, std::milli>(end - start_).count();
        if (duration_ms < 0.0)
        {
            duration_ms = 0.0;
        }
        Profiler::instance().record(id_, duration_ms);
    }

    ScopedIdTimer(const ScopedIdTimer&) = delete;
    auto operator=(const ScopedIdTimer&) -> ScopedIdTimer& = delete;
    ScopedIdTimer(ScopedIdTimer&&) = delete;
    auto operator=(ScopedIdTimer&&) -> ScopedIdTimer& = delete;

private:
    ProfileId id_;
    std::chrono::high_resolution_clock::time_point start_;
};

// Convenience macro — creates a unique variable per line
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_PROFILE_SCOPE(name)                                                                \
    auto _profiler_scope_##__LINE__ = markamp::core::Profiler::instance().scope(name)

// Phase 16: ID-based profiling macro — zero allocation fast path
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_PROFILE_SCOPE_ID(profile_id)                                                       \
    markamp::core::ScopedIdTimer _profiler_id_scope_##__LINE__(profile_id)

// ═══════════════════════════════════════════════════════
// Budget guard — debug-mode latency assertion
// ═══════════════════════════════════════════════════════

/// RAII budget guard that warns (via spdlog) if a scoped operation
/// exceeds a specified time budget. Use on the UI thread's hot path
/// to detect regressions early.
///
/// Pattern implemented: #11 O(1) typing path guarantee
/// Pattern implemented: #19 Instrumentation and performance budgets
class BudgetGuard
{
public:
    BudgetGuard(std::string_view name, std::chrono::microseconds budget);
    ~BudgetGuard();

    BudgetGuard(const BudgetGuard&) = delete;
    auto operator=(const BudgetGuard&) -> BudgetGuard& = delete;
    BudgetGuard(BudgetGuard&&) = delete;
    auto operator=(BudgetGuard&&) -> BudgetGuard& = delete;

private:
    std::string name_;
    std::chrono::microseconds budget_;
    std::chrono::high_resolution_clock::time_point start_;
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_BUDGET_GUARD(name, budget_us)                                                      \
    markamp::core::BudgetGuard _budget_guard_##__LINE__(name, std::chrono::microseconds(budget_us))

// ═══════════════════════════════════════════════════════
// Frame histogram — lock-free per-subsystem latency tracking
// ═══════════════════════════════════════════════════════

/// Lock-free histogram with 64 buckets (0–63ms, 1ms granularity).
/// Each record() is a single atomic fetch_add — safe for any thread.
///
/// Pattern implemented: #19 Instrumentation and performance budgets
class FrameHistogram
{
public:
    static constexpr std::size_t kBucketCount = 64;
    static constexpr double kBucketWidthMs = 1.0;

    FrameHistogram()
    {
        for (auto& bucket : buckets_)
        {
            bucket.store(0, std::memory_order_relaxed);
        }
    }

    /// Record a duration into the histogram.
    void record(double duration_ms) noexcept
    {
        // R20 Fix 20: Clamp negative values before size_t cast — avoids huge index
        if (duration_ms < 0.0)
        {
            duration_ms = 0.0;
        }
        auto bucket = static_cast<std::size_t>(duration_ms / kBucketWidthMs);
        if (bucket >= kBucketCount)
        {
            bucket = kBucketCount - 1;
        }
        buckets_[bucket].fetch_add(1, std::memory_order_relaxed);
    }

    /// Calculate the p-th percentile (0.0–1.0).
    [[nodiscard]] auto percentile(double percentile_value) const -> double
    {
        uint32_t total = 0;
        for (const auto& bucket : buckets_)
        {
            total += bucket.load(std::memory_order_relaxed);
        }
        if (total == 0)
        {
            return 0.0;
        }

        uint32_t target = static_cast<uint32_t>(static_cast<double>(total) * percentile_value);
        uint32_t cumulative = 0;

        for (std::size_t idx = 0; idx < kBucketCount; ++idx)
        {
            cumulative += buckets_[idx].load(std::memory_order_relaxed);
            if (cumulative >= target)
            {
                return static_cast<double>(idx) * kBucketWidthMs;
            }
        }
        return static_cast<double>(kBucketCount - 1) * kBucketWidthMs;
    }

    /// Total number of recorded samples.
    [[nodiscard]] auto total_count() const noexcept -> uint32_t
    {
        uint32_t total = 0;
        for (const auto& bucket : buckets_)
        {
            total += bucket.load(std::memory_order_relaxed);
        }
        return total;
    }

    /// Reset all buckets.
    void reset() noexcept
    {
        for (auto& bucket : buckets_)
        {
            bucket.store(0, std::memory_order_relaxed);
        }
    }

private:
    std::array<std::atomic<uint32_t>, kBucketCount> buckets_;
};

// ═══════════════════════════════════════════════════════
// Performance budget constants
// ═══════════════════════════════════════════════════════

/// Recommended budgets for critical UI operations.
/// Use with BudgetGuard to enforce latency SLAs.
///
/// Pattern implemented: #11 O(1) typing path guarantee
namespace budget
{

inline constexpr std::chrono::microseconds kKeystroke{500};         // 0.5ms per keystroke
inline constexpr std::chrono::microseconds kScroll{8000};           // 8ms per scroll frame
inline constexpr std::chrono::microseconds kPreviewUpdate{16000};   // 16ms for preview repaint
inline constexpr std::chrono::microseconds kFileOpen{100000};       // 100ms for file open
inline constexpr std::chrono::microseconds kSearchIteration{50000}; // 50ms per search batch

} // namespace budget

} // namespace markamp::core
