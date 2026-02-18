/// @file ObservabilityService.h
/// @brief V9 Phase 33 – Core observability: metrics, error rates, spans, log buffer, redaction.

#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// Log Level Configuration (PRD Task 3)
// ════════════════════════════════════════════════════════════════════

/// Configurable log levels for global and per-module overrides.
enum class ConfigurableLogLevel : uint8_t
{
    kDebug,
    kInfo,
    kWarn,
    kError,
};

class LogLevelConfig
{
public:
    /// Get the global minimum log level.
    [[nodiscard]] auto global_level() const -> ConfigurableLogLevel;

    /// Set the global minimum log level.
    void set_global_level(ConfigurableLogLevel level);

    /// Set a per-module override.
    void set_module_level(const std::string& module_name, ConfigurableLogLevel level);

    /// Remove a per-module override.
    void remove_module_level(const std::string& module_name);

    /// Get the effective level for a module (override or global).
    [[nodiscard]] auto effective_level(const std::string& module_name) const
        -> ConfigurableLogLevel;

    /// Check if a level should be logged for a module.
    [[nodiscard]] auto should_log(const std::string& module_name, ConfigurableLogLevel level) const
        -> bool;

    /// Get all module overrides.
    [[nodiscard]] auto module_overrides() const -> std::map<std::string, ConfigurableLogLevel>;

private:
    ConfigurableLogLevel global_level_{ConfigurableLogLevel::kInfo};
    std::map<std::string, ConfigurableLogLevel> module_levels_;
};

// ════════════════════════════════════════════════════════════════════
// Metrics Collection (PRD Task 5)
// ════════════════════════════════════════════════════════════════════

/// A single metric value (counter, gauge, or histogram).
enum class MetricType : uint8_t
{
    kCounter,
    kGauge,
    kHistogram,
};

struct MetricSnapshot
{
    std::string name;
    MetricType type{MetricType::kCounter};
    double value{0.0};                    ///< Current value (counter or gauge)
    std::vector<double> histogram_values; ///< All recorded values (histogram)
    int64_t last_updated_us{0};
};

class MetricsCollector
{
public:
    /// Increment a counter by delta (default 1).
    void increment(const std::string& name, double delta = 1.0);

    /// Set a gauge to an absolute value.
    void set(const std::string& name, double value);

    /// Record a value in a histogram.
    void record(const std::string& name, double value);

    /// Get a snapshot of a single metric.
    [[nodiscard]] auto get(const std::string& name) const -> MetricSnapshot;

    /// Get all metric snapshots.
    [[nodiscard]] auto all() const -> std::vector<MetricSnapshot>;

    /// Export all metrics as JSON string.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Reset all metrics.
    void reset();

    /// Check if a metric exists.
    [[nodiscard]] auto has(const std::string& name) const -> bool;

private:
    mutable std::mutex mutex_;
    std::map<std::string, MetricSnapshot> metrics_;

    auto ensure(const std::string& name, MetricType type) -> MetricSnapshot&;
};

// ════════════════════════════════════════════════════════════════════
// Error Rate Tracking (PRD Task 8)
// ════════════════════════════════════════════════════════════════════

struct ErrorRateAlert
{
    std::string module_name;
    int count{0};
    int threshold{0};
    int64_t timestamp_ms{0};
    bool is_spike{false};
};

class ErrorRateTracker
{
public:
    /// Record an error for a module.
    void record_error(const std::string& module_name);

    /// Get the current error count for a module in the current window.
    [[nodiscard]] auto error_count(const std::string& module_name) const -> int;

    /// Set the alert threshold (errors per window).
    void set_threshold(int threshold);

    /// Get the alert threshold.
    [[nodiscard]] auto threshold() const -> int;

    /// Check for alerts (modules exceeding threshold).
    [[nodiscard]] auto check_alerts() const -> std::vector<ErrorRateAlert>;

    /// Reset all counts.
    void reset();

    /// Get all module error counts.
    [[nodiscard]] auto all_counts() const -> std::map<std::string, int>;

private:
    mutable std::mutex mutex_;
    std::map<std::string, int> counts_;
    std::map<std::string, int> previous_counts_;
    int threshold_{10};
};

// ════════════════════════════════════════════════════════════════════
// Performance Span Tracking (PRD Task 9)
// ════════════════════════════════════════════════════════════════════

struct SpanRecord
{
    std::string name;
    int64_t start_us{0};
    int64_t duration_us{0};
    std::string parent_name;
    bool is_slow{false};
    bool completed{false};
};

class SpanTracker
{
public:
    /// Begin a span and return its index.
    auto begin_span(const std::string& name, const std::string& parent = "") -> size_t;

    /// End a span by index.
    void end_span(size_t span_index);

    /// Get all completed spans.
    [[nodiscard]] auto completed_spans() const -> std::vector<SpanRecord>;

    /// Set the slow span threshold in microseconds.
    void set_slow_threshold_us(int64_t threshold_us);

    /// Get the slow span threshold.
    [[nodiscard]] auto slow_threshold_us() const -> int64_t;

    /// Clear all spans.
    void clear();

private:
    mutable std::mutex mutex_;
    std::vector<SpanRecord> spans_;
    int64_t slow_threshold_us_{100'000}; ///< 100ms default
};

/// RAII span helper.
class PerformanceSpan
{
public:
    PerformanceSpan(SpanTracker& tracker, const std::string& name, const std::string& parent = "");
    ~PerformanceSpan();

    PerformanceSpan(const PerformanceSpan&) = delete;
    auto operator=(const PerformanceSpan&) -> PerformanceSpan& = delete;
    PerformanceSpan(PerformanceSpan&&) = delete;
    auto operator=(PerformanceSpan&&) -> PerformanceSpan& = delete;

private:
    SpanTracker& tracker_;
    size_t index_;
};

// ════════════════════════════════════════════════════════════════════
// Circular Log Buffer for Crash Context (PRD Task 15)
// ════════════════════════════════════════════════════════════════════

struct BufferedLogEntry
{
    std::string level;
    std::string component;
    std::string message;
    int64_t timestamp_us{0};
};

class LogBuffer
{
public:
    static constexpr size_t kDefaultCapacity = 500;

    explicit LogBuffer(size_t capacity = kDefaultCapacity);

    /// Push a log entry into the buffer.
    void push(const BufferedLogEntry& entry);

    /// Get all entries in chronological order.
    [[nodiscard]] auto entries() const -> std::vector<BufferedLogEntry>;

    /// Get the current count of entries.
    [[nodiscard]] auto size() const -> size_t;

    /// Get the buffer capacity.
    [[nodiscard]] auto capacity() const -> size_t;

    /// Clear the buffer.
    void clear();

    /// Export as JSON for crash reports.
    [[nodiscard]] auto export_json() const -> std::string;

private:
    mutable std::mutex mutex_;
    std::vector<BufferedLogEntry> buffer_;
    size_t head_{0};
    size_t count_{0};
    size_t capacity_;
};

// ════════════════════════════════════════════════════════════════════
// Log Redaction (PRD Task 17)
// ════════════════════════════════════════════════════════════════════

class LogRedactor
{
public:
    /// Redact a file path to relative form.
    [[nodiscard]] static auto redact_path(const std::string& path) -> std::string;

    /// Mask credentials in a string.
    [[nodiscard]] static auto redact_credentials(const std::string& text) -> std::string;

    /// Redact search queries (show length only).
    [[nodiscard]] static auto redact_query(const std::string& query) -> std::string;

    /// Full redaction pipeline on a log message.
    [[nodiscard]] static auto redact(const std::string& message) -> std::string;

    /// Check if a string contains sensitive patterns.
    [[nodiscard]] static auto contains_sensitive(const std::string& text) -> bool;
};

} // namespace markamp::core
