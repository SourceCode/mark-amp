/// ErrorReportingService.h — V7 Phase 30: Centralized error reporting & toast
///
/// Centralized error intake with deduplication, rate-limited toast
/// notifications, and integration with StructuredLogger.

#pragma once

#include "Result.h"
#include "StructuredLogger.h"

#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Error Report
// ══════════════════════════════════════════════════════════════════════════════

/// A deduplicated error report.
struct ErrorReport
{
    ErrorCode code{ErrorCode::Unknown};
    SubsystemId subsystem{SubsystemId::Core};
    std::string message;
    size_t occurrence_count{1};
    int64_t first_seen_ms{0};
    int64_t last_seen_ms{0};
    bool notified{false};
};

// ══════════════════════════════════════════════════════════════════════════════
// Error Reporting Service
// ══════════════════════════════════════════════════════════════════════════════

/// Centralized error intake with deduplication and rate-limited notifications.
class ErrorReportingService
{
public:
    ErrorReportingService() = default;

    /// Report an error. Deduplicates by code + subsystem.
    void report(const Error& error);

    /// Register a toast notification callback.
    void set_toast_callback(std::function<void(const ErrorReport&)> callback);

    /// Get all current error reports.
    [[nodiscard]] auto all_reports() const -> std::vector<ErrorReport>;

    /// Get the total number of unique errors.
    [[nodiscard]] auto unique_error_count() const -> size_t;

    /// Get the total number of error occurrences.
    [[nodiscard]] auto total_occurrences() const -> size_t;

    /// Check if an error code has been reported.
    [[nodiscard]] auto has_error(ErrorCode code, SubsystemId subsystem) const -> bool;

    /// Clear all reports.
    void clear();

    /// Set the minimum interval between toast notifications for the same error.
    void set_toast_cooldown(std::chrono::seconds cooldown)
    {
        toast_cooldown_ = cooldown;
    }

    /// Set the maximum number of toasts per time window.
    void set_max_toasts_per_window(size_t count)
    {
        max_toasts_per_window_ = count;
    }

private:
    /// Deduplication key: code + subsystem.
    [[nodiscard]] static auto make_key(ErrorCode code, SubsystemId subsystem) -> std::string;

    /// Check if a toast should be shown (rate limiting).
    [[nodiscard]] auto should_show_toast() -> bool;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, ErrorReport> reports_;
    std::function<void(const ErrorReport&)> toast_callback_;

    std::chrono::seconds toast_cooldown_{30};
    size_t max_toasts_per_window_{5};
    std::deque<std::chrono::steady_clock::time_point> recent_toasts_;

    StructuredLogger logger_{"ErrorReporting"};
};

} // namespace markamp::core
