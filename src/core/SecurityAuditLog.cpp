// ============================================================================
// File: src/core/SecurityAuditLog.cpp
// Phase 29: Security & Input Validation — Centralized Security Audit Log
// ============================================================================

#include "SecurityAuditLog.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace markamp::core
{

// ── Construction ──

SecurityAuditLog::SecurityAuditLog(size_t max_entries)
    : max_entries_(max_entries)
{
    entries_.reserve(max_entries_);
}

// ── Recording ──

auto SecurityAuditLog::record(const SecurityEvent& event) -> void
{
    const std::lock_guard lock(mutex_);

    if (entries_.size() < max_entries_)
    {
        entries_.push_back(event);
    }
    else
    {
        entries_[write_index_] = event;
    }
    write_index_ = (write_index_ + 1) % max_entries_;
    ++total_recorded_;
}

auto SecurityAuditLog::record(SecuritySeverity severity,
                              SecurityCategory category,
                              const std::string& description,
                              const std::string& source,
                              const std::string& details) -> void
{
    SecurityEvent event;
    event.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
    event.severity = severity;
    event.category = category;
    event.description = description;
    event.source_component = source;
    event.details = details;

    record(event);
}

// ── Querying ──

auto SecurityAuditLog::ordered_entries() const -> std::vector<SecurityEvent>
{
    if (entries_.size() < max_entries_)
    {
        return entries_;
    }

    // Circular buffer is full — reconstruct in order.
    std::vector<SecurityEvent> ordered;
    ordered.reserve(max_entries_);
    for (size_t idx = 0; idx < max_entries_; ++idx)
    {
        ordered.push_back(entries_[(write_index_ + idx) % max_entries_]);
    }
    return ordered;
}

auto SecurityAuditLog::query(const SecurityEventFilter& filter) const -> std::vector<SecurityEvent>
{
    const std::lock_guard lock(mutex_);
    auto kAll = ordered_entries();

    std::vector<SecurityEvent> result;
    for (const auto& entry : kAll)
    {
        if (static_cast<uint8_t>(entry.severity) < static_cast<uint8_t>(filter.min_severity))
        {
            continue;
        }
        if (filter.filter_by_category && entry.category != filter.category)
        {
            continue;
        }
        if (filter.from_timestamp_ms > 0 && entry.timestamp_ms < filter.from_timestamp_ms)
        {
            continue;
        }
        if (filter.to_timestamp_ms > 0 && entry.timestamp_ms > filter.to_timestamp_ms)
        {
            continue;
        }
        result.push_back(entry);
    }
    return result;
}

auto SecurityAuditLog::recent(size_t count) const -> std::vector<SecurityEvent>
{
    const std::lock_guard lock(mutex_);
    auto kAll = ordered_entries();

    if (count >= kAll.size())
    {
        return kAll;
    }

    return {kAll.end() - static_cast<std::ptrdiff_t>(count), kAll.end()};
}

// ── Summary ──

auto SecurityAuditLog::summary() const -> SecuritySummary
{
    const std::lock_guard lock(mutex_);
    SecuritySummary result;
    result.total_events = static_cast<int32_t>(entries_.size());

    for (const auto& entry : entries_)
    {
        switch (entry.severity)
        {
            case SecuritySeverity::kInfo:
                ++result.info_count;
                break;
            case SecuritySeverity::kWarning:
                ++result.warning_count;
                break;
            case SecuritySeverity::kCritical:
                ++result.critical_count;
                if (entry.timestamp_ms > result.last_critical_timestamp_ms)
                {
                    result.last_critical_timestamp_ms = entry.timestamp_ms;
                }
                break;
        }

        switch (entry.category)
        {
            case SecurityCategory::kAuth:
                ++result.auth_events;
                break;
            case SecurityCategory::kAccess:
                ++result.access_events;
                break;
            case SecurityCategory::kInput:
                ++result.input_events;
                break;
            case SecurityCategory::kNetwork:
                ++result.network_events;
                break;
            case SecurityCategory::kExtension:
                ++result.extension_events;
                break;
            case SecurityCategory::kEncryption:
            case SecurityCategory::kPolicy:
                break;
        }
    }
    return result;
}

auto SecurityAuditLog::size() const -> size_t
{
    const std::lock_guard lock(mutex_);
    return entries_.size();
}

auto SecurityAuditLog::clear() -> void
{
    const std::lock_guard lock(mutex_);
    entries_.clear();
    write_index_ = 0;
    total_recorded_ = 0;
}

auto SecurityAuditLog::max_entries() const noexcept -> size_t
{
    return max_entries_;
}

// ── JSON export ──

auto SecurityAuditLog::export_json() const -> std::string
{
    const std::lock_guard lock(mutex_);
    auto kAll = ordered_entries();

    std::ostringstream oss;
    oss << R"([)" << '\n';

    for (size_t idx = 0; idx < kAll.size(); ++idx)
    {
        const auto& entry = kAll[idx];
        oss << R"(  {)" << '\n';
        oss << R"(    "timestamp_ms": )" << entry.timestamp_ms << ",\n";

        auto severity_str = [](SecuritySeverity sev) -> std::string_view
        {
            switch (sev)
            {
                case SecuritySeverity::kInfo:
                    return "info";
                case SecuritySeverity::kWarning:
                    return "warning";
                case SecuritySeverity::kCritical:
                    return "critical";
            }
            return "unknown";
        };

        auto category_str = [](SecurityCategory cat) -> std::string_view
        {
            switch (cat)
            {
                case SecurityCategory::kAuth:
                    return "auth";
                case SecurityCategory::kAccess:
                    return "access";
                case SecurityCategory::kInput:
                    return "input";
                case SecurityCategory::kNetwork:
                    return "network";
                case SecurityCategory::kExtension:
                    return "extension";
                case SecurityCategory::kEncryption:
                    return "encryption";
                case SecurityCategory::kPolicy:
                    return "policy";
            }
            return "unknown";
        };

        oss << R"(    "severity": ")" << severity_str(entry.severity) << R"(",)" << '\n';
        oss << R"(    "category": ")" << category_str(entry.category) << R"(",)" << '\n';
        oss << R"(    "description": ")" << entry.description << R"(",)" << '\n';
        oss << R"(    "source": ")" << entry.source_component << R"(",)" << '\n';
        oss << R"(    "details": ")" << entry.details << R"(")" << '\n';

        if (idx + 1 < kAll.size())
        {
            oss << R"(  },)" << '\n';
        }
        else
        {
            oss << R"(  })" << '\n';
        }
    }

    oss << "]";
    return oss.str();
}

} // namespace markamp::core
