/// ErrorReportingService.cpp — V7 Phase 30: Centralized error reporting

#include "ErrorReportingService.h"

namespace markamp::core
{

void ErrorReportingService::report(const Error& error)
{
    std::lock_guard lock(mutex_);

    auto key = make_key(error.code, error.subsystem);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    auto report_it = reports_.find(key);
    if (report_it != reports_.end())
    {
        // Update existing report
        report_it->second.occurrence_count++;
        report_it->second.last_seen_ms = now_ms;
        report_it->second.message = error.message; // Keep latest message
    }
    else
    {
        // New report
        reports_[key] = ErrorReport{
            .code = error.code,
            .subsystem = error.subsystem,
            .message = error.message,
            .occurrence_count = 1,
            .first_seen_ms = now_ms,
            .last_seen_ms = now_ms,
            .notified = false,
        };
    }

    // Fire toast if callback set and rate limit allows
    if (toast_callback_ && should_show_toast())
    {
        auto& report = reports_[key];
        report.notified = true;
        toast_callback_(report);
    }

    logger_.warn("Error reported: " + error.message,
                 {{"code", std::string(error_code_name(error.code))},
                  {"subsystem", std::string(subsystem_name(error.subsystem))}});
}

void ErrorReportingService::set_toast_callback(std::function<void(const ErrorReport&)> callback)
{
    std::lock_guard lock(mutex_);
    toast_callback_ = std::move(callback);
}

auto ErrorReportingService::all_reports() const -> std::vector<ErrorReport>
{
    std::lock_guard lock(mutex_);
    std::vector<ErrorReport> result;
    result.reserve(reports_.size());
    for (const auto& [dedup_key, report] : reports_)
    {
        result.push_back(report);
    }
    return result;
}

auto ErrorReportingService::unique_error_count() const -> size_t
{
    std::lock_guard lock(mutex_);
    return reports_.size();
}

auto ErrorReportingService::total_occurrences() const -> size_t
{
    std::lock_guard lock(mutex_);
    size_t total = 0;
    for (const auto& [dedup_key, report] : reports_)
    {
        total += report.occurrence_count;
    }
    return total;
}

auto ErrorReportingService::has_error(ErrorCode code, SubsystemId subsystem) const -> bool
{
    std::lock_guard lock(mutex_);
    return reports_.contains(make_key(code, subsystem));
}

void ErrorReportingService::clear()
{
    std::lock_guard lock(mutex_);
    reports_.clear();
    recent_toasts_.clear();
}

auto ErrorReportingService::make_key(ErrorCode code, SubsystemId subsystem) -> std::string
{
    return std::to_string(static_cast<int>(code)) + ":" +
           std::to_string(static_cast<int>(subsystem));
}

auto ErrorReportingService::should_show_toast() -> bool
{
    auto now = std::chrono::steady_clock::now();

    // Remove toasts older than cooldown window
    auto window_start = now - toast_cooldown_;
    while (!recent_toasts_.empty() && recent_toasts_.front() < window_start)
    {
        recent_toasts_.pop_front();
    }

    // Check rate limit
    if (recent_toasts_.size() >= max_toasts_per_window_)
    {
        return false;
    }

    recent_toasts_.push_back(now);
    return true;
}

} // namespace markamp::core
