/// @file ThemePerformanceMonitor.h
/// @brief V9 Phase 3 — Theme performance metrics tracking.

#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

/// Performance metrics for theme operations.
struct ThemePerformanceMetrics
{
    std::chrono::microseconds last_rebuild_cache_us{0};
    std::chrono::microseconds last_apply_window_us{0};
    std::chrono::microseconds last_font_build_us{0};
    std::size_t total_cache_rebuilds{0};
    std::size_t total_window_applies{0};
    double avg_rebuild_cache_us{0.0};
};

/// Track and report theme engine performance.
class ThemePerformanceMonitor
{
public:
    void record_cache_rebuild(std::chrono::microseconds duration);
    void record_window_apply(std::chrono::microseconds duration);
    void record_font_build(std::chrono::microseconds duration);

    [[nodiscard]] auto metrics() const -> const ThemePerformanceMetrics&;
    void reset();

private:
    ThemePerformanceMetrics metrics_;
};

} // namespace markamp::core
