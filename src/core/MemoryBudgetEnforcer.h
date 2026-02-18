// ============================================================================
// File: src/core/MemoryBudgetEnforcer.h
// Phase 30: Performance Optimization — Per-subsystem memory budget tracking
// ============================================================================
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Subsystem identifiers for memory budget tracking.
enum class MemorySubsystem : uint8_t
{
    kEditor = 0,
    kCanvas,
    kSearchIndex,
    kExtensions,
    kGeneral,

    kCount // sentinel
};

/// Human-readable names for memory subsystems.
[[nodiscard]] constexpr auto subsystem_name(MemorySubsystem sub) noexcept -> std::string_view
{
    constexpr std::array<std::string_view, static_cast<size_t>(MemorySubsystem::kCount)> kNames = {{
        "Editor",
        "Canvas",
        "SearchIndex",
        "Extensions",
        "General",
    }};
    const auto idx = static_cast<size_t>(sub);
    if (idx < kNames.size())
    {
        return kNames.at(idx);
    }
    return "Unknown";
}

/// Budget threshold severity.
enum class BudgetSeverity : uint8_t
{
    kNormal = 0, // < 80%
    kWarning,    // >= 80%, < 95%
    kCritical,   // >= 95%
};

/// Per-subsystem memory usage snapshot.
struct SubsystemMemoryInfo
{
    MemorySubsystem subsystem{MemorySubsystem::kGeneral};
    size_t usage_bytes{0};
    size_t budget_bytes{0};
    double usage_percent{0.0};
    BudgetSeverity severity{BudgetSeverity::kNormal};
};

/// Complete memory snapshot across all subsystems.
struct MemorySnapshot
{
    std::array<SubsystemMemoryInfo, static_cast<size_t>(MemorySubsystem::kCount)> subsystems{};
    size_t total_usage{0};
    size_t total_budget{0};
    double total_usage_percent{0.0};
    int64_t timestamp_ms{0};
};

/// Per-subsystem memory tracking with configurable budgets.
///
/// Tracks allocations per subsystem, warns at 80% usage, critical at 95%.
/// Default budgets per PRD Task 12:
///   Editor: 50MB, Canvas: 200MB, SearchIndex: 100MB,
///   Extensions: 50MB, General: 100MB  (Total: 500MB)
class MemoryBudgetEnforcer
{
public:
    // Default budgets in bytes (per PRD)
    static constexpr size_t kEditorBudget = 50ULL * 1024 * 1024;       // 50 MB
    static constexpr size_t kCanvasBudget = 200ULL * 1024 * 1024;      // 200 MB
    static constexpr size_t kSearchIndexBudget = 100ULL * 1024 * 1024; // 100 MB
    static constexpr size_t kExtensionsBudget = 50ULL * 1024 * 1024;   // 50 MB
    static constexpr size_t kGeneralBudget = 100ULL * 1024 * 1024;     // 100 MB

    static constexpr double kWarningThreshold = 0.80;
    static constexpr double kCriticalThreshold = 0.95;

    MemoryBudgetEnforcer();

    /// Record an allocation for a subsystem.
    void allocate(MemorySubsystem sub, size_t bytes);

    /// Record a deallocation for a subsystem.
    void release(MemorySubsystem sub, size_t bytes);

    /// Current usage for a subsystem in bytes.
    [[nodiscard]] auto usage(MemorySubsystem sub) const -> size_t;

    /// Current usage as percentage of budget (0.0–1.0).
    [[nodiscard]] auto usage_percent(MemorySubsystem sub) const -> double;

    /// Current budget severity for a subsystem.
    [[nodiscard]] auto severity(MemorySubsystem sub) const -> BudgetSeverity;

    /// Get the budget for a subsystem.
    [[nodiscard]] auto budget(MemorySubsystem sub) const -> size_t;

    /// Set a custom budget for a subsystem.
    void set_budget(MemorySubsystem sub, size_t bytes);

    /// Total usage across all subsystems.
    [[nodiscard]] auto total_usage() const -> size_t;

    /// Total budget across all subsystems.
    [[nodiscard]] auto total_budget() const -> size_t;

    /// Take a complete memory snapshot.
    [[nodiscard]] auto snapshot() const -> MemorySnapshot;

    /// Export snapshot as JSON string.
    [[nodiscard]] auto to_json() const -> std::string;

    /// Reset all usage counters to zero.
    void reset();

private:
    struct SubsystemState
    {
        size_t usage{0};
        size_t budget{0};
    };

    std::array<SubsystemState, static_cast<size_t>(MemorySubsystem::kCount)> subsystems_{};
};

} // namespace markamp::core
