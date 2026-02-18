/// @file ObservabilityService.cpp
/// @brief V9 Phase 33 – Observability core implementation.

#include "ObservabilityService.h"

#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// LogLevelConfig
// ════════════════════════════════════════════════════════════════════

auto LogLevelConfig::global_level() const -> ConfigurableLogLevel
{
    return global_level_;
}

void LogLevelConfig::set_global_level(ConfigurableLogLevel level)
{
    global_level_ = level;
}

void LogLevelConfig::set_module_level(const std::string& module_name, ConfigurableLogLevel level)
{
    module_levels_[module_name] = level;
}

void LogLevelConfig::remove_module_level(const std::string& module_name)
{
    module_levels_.erase(module_name);
}

auto LogLevelConfig::effective_level(const std::string& module_name) const -> ConfigurableLogLevel
{
    auto iter = module_levels_.find(module_name);
    if (iter != module_levels_.end())
    {
        return iter->second;
    }
    return global_level_;
}

auto LogLevelConfig::should_log(const std::string& module_name, ConfigurableLogLevel level) const
    -> bool
{
    return static_cast<uint8_t>(level) >= static_cast<uint8_t>(effective_level(module_name));
}

auto LogLevelConfig::module_overrides() const -> std::map<std::string, ConfigurableLogLevel>
{
    return module_levels_;
}

// ════════════════════════════════════════════════════════════════════
// MetricsCollector
// ════════════════════════════════════════════════════════════════════

void MetricsCollector::increment(const std::string& name, double delta)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& metric = ensure(name, MetricType::kCounter);
    metric.value += delta;
    metric.last_updated_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();
}

void MetricsCollector::set(const std::string& name, double value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& metric = ensure(name, MetricType::kGauge);
    metric.value = value;
    metric.last_updated_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();
}

void MetricsCollector::record(const std::string& name, double value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& metric = ensure(name, MetricType::kHistogram);
    metric.histogram_values.push_back(value);
    metric.value = value; // last recorded value
    metric.last_updated_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count();
}

auto MetricsCollector::get(const std::string& name) const -> MetricSnapshot
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = metrics_.find(name);
    if (iter != metrics_.end())
    {
        return iter->second;
    }
    return {};
}

auto MetricsCollector::all() const -> std::vector<MetricSnapshot>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<MetricSnapshot> result;
    result.reserve(metrics_.size());
    for (const auto& [metric_name, snapshot] : metrics_)
    {
        result.push_back(snapshot);
    }
    return result;
}

auto MetricsCollector::export_json() const -> std::string
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "{\"metrics\":[";
    bool first = true;
    for (const auto& [metric_name, snapshot] : metrics_)
    {
        if (!first)
        {
            oss << ',';
        }
        first = false;
        oss << "{\"name\":\"" << metric_name << "\",\"type\":";
        switch (snapshot.type)
        {
            case MetricType::kCounter:
                oss << "\"counter\"";
                break;
            case MetricType::kGauge:
                oss << "\"gauge\"";
                break;
            case MetricType::kHistogram:
                oss << "\"histogram\"";
                break;
        }
        oss << ",\"value\":" << snapshot.value;
        if (snapshot.type == MetricType::kHistogram && !snapshot.histogram_values.empty())
        {
            oss << ",\"count\":" << snapshot.histogram_values.size();
        }
        oss << '}';
    }
    oss << "]}";
    return oss.str();
}

void MetricsCollector::reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.clear();
}

auto MetricsCollector::has(const std::string& name) const -> bool
{
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_.count(name) > 0;
}

auto MetricsCollector::ensure(const std::string& name, MetricType type) -> MetricSnapshot&
{
    auto iter = metrics_.find(name);
    if (iter == metrics_.end())
    {
        MetricSnapshot snapshot;
        snapshot.name = name;
        snapshot.type = type;
        auto [inserted, success] = metrics_.emplace(name, snapshot);
        return inserted->second;
    }
    return iter->second;
}

// ════════════════════════════════════════════════════════════════════
// ErrorRateTracker
// ════════════════════════════════════════════════════════════════════

void ErrorRateTracker::record_error(const std::string& module_name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    counts_[module_name]++;
}

auto ErrorRateTracker::error_count(const std::string& module_name) const -> int
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = counts_.find(module_name);
    if (iter != counts_.end())
    {
        return iter->second;
    }
    return 0;
}

void ErrorRateTracker::set_threshold(int error_threshold)
{
    threshold_ = error_threshold;
}

auto ErrorRateTracker::threshold() const -> int
{
    return threshold_;
}

auto ErrorRateTracker::check_alerts() const -> std::vector<ErrorRateAlert>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ErrorRateAlert> alerts;

    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    for (const auto& [module_name, count] : counts_)
    {
        if (count >= threshold_)
        {
            ErrorRateAlert alert;
            alert.module_name = module_name;
            alert.count = count;
            alert.threshold = threshold_;
            alert.timestamp_ms = now_ms;

            // Check for spike: count jumped significantly from previous window
            auto prev_iter = previous_counts_.find(module_name);
            if (prev_iter != previous_counts_.end() && prev_iter->second > 0)
            {
                alert.is_spike = (count > prev_iter->second * 2);
            }
            else
            {
                alert.is_spike = true; // First window with errors is a spike
            }

            alerts.push_back(alert);
        }
    }
    return alerts;
}

void ErrorRateTracker::reset()
{
    std::lock_guard<std::mutex> lock(mutex_);
    previous_counts_ = counts_;
    counts_.clear();
}

auto ErrorRateTracker::all_counts() const -> std::map<std::string, int>
{
    std::lock_guard<std::mutex> lock(mutex_);
    return counts_;
}

// ════════════════════════════════════════════════════════════════════
// SpanTracker
// ════════════════════════════════════════════════════════════════════

auto SpanTracker::begin_span(const std::string& name, const std::string& parent) -> size_t
{
    std::lock_guard<std::mutex> lock(mutex_);
    SpanRecord record;
    record.name = name;
    record.parent_name = parent;
    record.start_us = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch())
                          .count();
    spans_.push_back(record);
    return spans_.size() - 1;
}

void SpanTracker::end_span(size_t span_index)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    if (span_index < spans_.size())
    {
        auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch())
                          .count();
        spans_[span_index].duration_us = now_us - spans_[span_index].start_us;
        spans_[span_index].is_slow = (spans_[span_index].duration_us > slow_threshold_us_);
        spans_[span_index].completed = true;
    }
}

auto SpanTracker::completed_spans() const -> std::vector<SpanRecord>
{
    const std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SpanRecord> result;
    for (const auto& span : spans_)
    {
        if (span.completed)
        {
            result.push_back(span);
        }
    }
    return result;
}

void SpanTracker::set_slow_threshold_us(int64_t threshold_us)
{
    slow_threshold_us_ = threshold_us;
}

auto SpanTracker::slow_threshold_us() const -> int64_t
{
    return slow_threshold_us_;
}

void SpanTracker::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    spans_.clear();
}

// PerformanceSpan RAII

PerformanceSpan::PerformanceSpan(SpanTracker& tracker,
                                 const std::string& name,
                                 const std::string& parent)
    : tracker_(tracker)
    , index_(tracker.begin_span(name, parent))
{
}

PerformanceSpan::~PerformanceSpan()
{
    tracker_.end_span(index_);
}

// ════════════════════════════════════════════════════════════════════
// LogBuffer
// ════════════════════════════════════════════════════════════════════

LogBuffer::LogBuffer(size_t cap)
    : capacity_(cap)
{
    buffer_.resize(capacity_);
}

void LogBuffer::push(const BufferedLogEntry& entry)
{
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_[head_] = entry;
    head_ = (head_ + 1) % capacity_;
    if (count_ < capacity_)
    {
        ++count_;
    }
}

auto LogBuffer::entries() const -> std::vector<BufferedLogEntry>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BufferedLogEntry> result;
    result.reserve(count_);

    if (count_ < capacity_)
    {
        // Buffer not yet full — entries start at 0
        for (size_t idx = 0; idx < count_; ++idx)
        {
            result.push_back(buffer_[idx]);
        }
    }
    else
    {
        // Buffer full — head_ points to oldest entry
        for (size_t idx = 0; idx < capacity_; ++idx)
        {
            result.push_back(buffer_[(head_ + idx) % capacity_]);
        }
    }
    return result;
}

auto LogBuffer::size() const -> size_t
{
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

auto LogBuffer::capacity() const -> size_t
{
    return capacity_;
}

void LogBuffer::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    count_ = 0;
    head_ = 0;
}

auto LogBuffer::export_json() const -> std::string
{
    auto all_entries = entries();
    std::ostringstream oss;
    oss << "{\"log_buffer\":[";
    bool first = true;
    for (const auto& entry : all_entries)
    {
        if (!first)
        {
            oss << ',';
        }
        first = false;
        oss << "{\"level\":\"" << entry.level << "\",\"component\":\"" << entry.component
            << "\",\"msg\":\"" << entry.message << "\",\"ts\":" << entry.timestamp_us << '}';
    }
    oss << "]}";
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════
// LogRedactor
// ════════════════════════════════════════════════════════════════════

auto LogRedactor::redact_path(const std::string& path) -> std::string
{
    // Keep only the last component (filename)
    auto last_sep = path.find_last_of("/\\");
    if (last_sep != std::string::npos && last_sep + 1 < path.size())
    {
        return "./" + path.substr(last_sep + 1);
    }
    return path;
}

auto LogRedactor::redact_credentials(const std::string& text) -> std::string
{
    std::string result = text;

    // Look for common credential patterns and mask them
    // Pattern: key=value or key: value where key suggests credential
    const std::vector<std::string> kSensitiveKeys = {
        "password",
        "token",
        "secret",
        "api_key",
        "apikey",
        "auth",
        "credential",
        "private_key",
        "access_key",
    };

    for (const auto& key : kSensitiveKeys)
    {
        // Case-insensitive search and mask
        std::string lower_result = result;
        std::transform(lower_result.begin(),
                       lower_result.end(),
                       lower_result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        auto pos = lower_result.find(key);
        while (pos != std::string::npos)
        {
            // Find the value portion after = or : or "
            auto val_start = result.find_first_of("=:\"", pos + key.size());
            if (val_start != std::string::npos)
            {
                val_start++;
                // Skip whitespace
                while (val_start < result.size() && result[val_start] == ' ')
                {
                    val_start++;
                }
                auto val_end = result.find_first_of(" ,;}\"\n", val_start);
                if (val_end == std::string::npos)
                {
                    val_end = result.size();
                }
                result.replace(val_start, val_end - val_start, "***");
                // Re-compute lower_result after modification
                lower_result = result;
                std::transform(lower_result.begin(),
                               lower_result.end(),
                               lower_result.begin(),
                               [](unsigned char chr)
                               { return static_cast<char>(std::tolower(chr)); });
            }
            pos = lower_result.find(key, pos + key.size());
        }
    }
    return result;
}

auto LogRedactor::redact_query(const std::string& query) -> std::string
{
    return "<query length=" + std::to_string(query.size()) + ">";
}

auto LogRedactor::redact(const std::string& message) -> std::string
{
    std::string result = message;
    if (contains_sensitive(result))
    {
        result = redact_credentials(result);
    }
    return result;
}

auto LogRedactor::contains_sensitive(const std::string& text) -> bool
{
    std::string lower = text;
    std::transform(lower.begin(),
                   lower.end(),
                   lower.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    const std::vector<std::string> kPatterns = {
        "password",
        "token",
        "secret",
        "api_key",
        "apikey",
        "auth",
        "credential",
        "private_key",
    };

    return std::any_of(kPatterns.begin(),
                       kPatterns.end(),
                       [&lower](const std::string& pattern)
                       { return lower.find(pattern) != std::string::npos; });
}

} // namespace markamp::core
