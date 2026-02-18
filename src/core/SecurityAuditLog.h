// ============================================================================
// File: src/core/SecurityAuditLog.h
// Phase 29: Security & Input Validation — Centralized Security Audit Log
// ============================================================================
#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace markamp::core
{

/// Severity of a security event.
enum class SecuritySeverity : uint8_t
{
    kInfo,
    kWarning,
    kCritical,
};

/// Category of a security event.
enum class SecurityCategory : uint8_t
{
    kAuth,       ///< Authentication events
    kAccess,     ///< Access control events
    kInput,      ///< Input validation events
    kNetwork,    ///< Network security events
    kExtension,  ///< Extension/plugin security events
    kEncryption, ///< Encryption-related events
    kPolicy,     ///< Policy enforcement events
};

/// A single security audit log entry.
struct SecurityEvent
{
    int64_t timestamp_ms{0};
    SecuritySeverity severity{SecuritySeverity::kInfo};
    SecurityCategory category{SecurityCategory::kInput};
    std::string description;
    std::string source_component;
    std::string details;
};

/// Summary statistics of the security audit log.
struct SecuritySummary
{
    int32_t total_events{0};
    int32_t info_count{0};
    int32_t warning_count{0};
    int32_t critical_count{0};
    int64_t last_critical_timestamp_ms{0};
    int32_t auth_events{0};
    int32_t access_events{0};
    int32_t input_events{0};
    int32_t network_events{0};
    int32_t extension_events{0};
};

/// Filter criteria for querying the audit log.
struct SecurityEventFilter
{
    SecuritySeverity min_severity{SecuritySeverity::kInfo};
    SecurityCategory category{
        SecurityCategory::kInput}; ///< Only used if filter_by_category == true
    bool filter_by_category{false};
    int64_t from_timestamp_ms{0};
    int64_t to_timestamp_ms{0}; ///< 0 = no upper bound
};

/// Centralized in-memory security audit log.
///
/// Thread-safe circular buffer of security events. Records security-relevant
/// actions across all security components for diagnostics and compliance.
class SecurityAuditLog
{
public:
    /// Construct with a given maximum capacity (default: 500 entries).
    explicit SecurityAuditLog(size_t max_entries = 500);

    /// Record a security event.
    auto record(const SecurityEvent& event) -> void;

    /// Convenience: record with severity, category, description, and source.
    auto record(SecuritySeverity severity,
                SecurityCategory category,
                const std::string& description,
                const std::string& source = "",
                const std::string& details = "") -> void;

    /// Query events matching a filter.
    [[nodiscard]] auto query(const SecurityEventFilter& filter) const -> std::vector<SecurityEvent>;

    /// Get the N most recent events.
    [[nodiscard]] auto recent(size_t count) const -> std::vector<SecurityEvent>;

    /// Get a summary of the log.
    [[nodiscard]] auto summary() const -> SecuritySummary;

    /// Get the total number of events in the log.
    [[nodiscard]] auto size() const -> size_t;

    /// Clear the log.
    auto clear() -> void;

    /// Export the log as a JSON string.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Get the maximum capacity.
    [[nodiscard]] auto max_entries() const noexcept -> size_t;

private:
    mutable std::mutex mutex_;
    std::vector<SecurityEvent> entries_;
    size_t max_entries_;
    size_t write_index_{0};
    size_t total_recorded_{0};

    /// Get all events in chronological order (must hold lock).
    [[nodiscard]] auto ordered_entries() const -> std::vector<SecurityEvent>;
};

} // namespace markamp::core
