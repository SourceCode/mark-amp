/// @file ThemePerformanceMonitor.cpp
/// @brief V9 Phase 3 — Theme performance metrics implementation.

#include "core/ThemePerformanceMonitor.h"

namespace markamp::core
{

void ThemePerformanceMonitor::record_cache_rebuild(std::chrono::microseconds duration)
{
    metrics_.last_rebuild_cache_us = duration;
    ++metrics_.total_cache_rebuilds;
    // Running average
    metrics_.avg_rebuild_cache_us =
        ((metrics_.avg_rebuild_cache_us * static_cast<double>(metrics_.total_cache_rebuilds - 1)) +
         static_cast<double>(duration.count())) /
        static_cast<double>(metrics_.total_cache_rebuilds);
}

void ThemePerformanceMonitor::record_window_apply(std::chrono::microseconds duration)
{
    metrics_.last_apply_window_us = duration;
    ++metrics_.total_window_applies;
}

void ThemePerformanceMonitor::record_font_build(std::chrono::microseconds duration)
{
    metrics_.last_font_build_us = duration;
}

auto ThemePerformanceMonitor::metrics() const -> const ThemePerformanceMetrics&
{
    return metrics_;
}

void ThemePerformanceMonitor::reset()
{
    metrics_ = ThemePerformanceMetrics{};
}

// ── Batch 23-25 (#145-146) ──

auto ThemePerformanceMonitor::total_operations() const -> std::size_t
{
    return metrics_.total_cache_rebuilds + metrics_.total_window_applies;
}

auto ThemePerformanceMonitor::avg_cache_rebuild_us() const -> double
{
    return metrics_.avg_rebuild_cache_us;
}

} // namespace markamp::core
