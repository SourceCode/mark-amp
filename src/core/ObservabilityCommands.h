/// @file ObservabilityCommands.h
/// @brief V9 Phase 33 – User-facing observability: commands, channels, action logs, filtering.

#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// MARKAMP_LOG Macro Routing (PRD Task 2)
// ════════════════════════════════════════════════════════════════════

/// Module name extracted from file path for MARKAMP_LOG routing.
/// Returns only the last component (e.g., "editor" from "src/core/Editor.cpp").
[[nodiscard]] auto extract_module_name(const char* file_path) -> std::string;

// ════════════════════════════════════════════════════════════════════
// Log Channel Registry (PRD Tasks 4, wiring predefined channels)
// ════════════════════════════════════════════════════════════════════

/// Predefined log channel names for subsystems.
struct LogChannels
{
    static constexpr const char* kGeneral = "General";
    static constexpr const char* kEditor = "Editor";
    static constexpr const char* kSync = "Sync";
    static constexpr const char* kPerformance = "Performance";
    static constexpr const char* kSecurity = "Security";
    static constexpr const char* kExtensions = "Extensions";
    static constexpr const char* kActivity = "Activity";
    static constexpr const char* kHealth = "Health";

    /// Get all predefined channel names.
    [[nodiscard]] static auto all() -> std::vector<std::string>;
};

// ════════════════════════════════════════════════════════════════════
// User Action Logger (PRD Task 12)
// ════════════════════════════════════════════════════════════════════

enum class UserActionType : uint8_t
{
    kCommand,
    kFileOpen,
    kSurfaceTransition,
    kSearch,
    kFileSave,
};

struct UserAction
{
    UserActionType type{UserActionType::kCommand};
    std::string action_name;
    int64_t timestamp_ms{0};
};

class UserActionLogger
{
public:
    /// Log a user action (only if enabled).
    void log_action(UserActionType type, const std::string& action_name);

    /// Enable or disable action logging.
    void set_enabled(bool enabled);

    /// Check if action logging is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Get recent actions.
    [[nodiscard]] auto recent_actions(size_t count = 100) const -> std::vector<UserAction>;

    /// Clear all logged actions.
    void clear();

    /// Export actions as JSON.
    [[nodiscard]] auto export_json() const -> std::string;

private:
    bool enabled_{false}; ///< Disabled by default (opt-in)
    std::vector<UserAction> actions_;
};

// ════════════════════════════════════════════════════════════════════
// Log Filter Service (PRD Task 13)
// ════════════════════════════════════════════════════════════════════

enum class LogTimeRange : uint8_t
{
    kLast5Minutes,
    kLastHour,
    kAll,
};

struct LogFilterCriteria
{
    std::string search_text;
    std::string level_filter; ///< "" = all, else "INFO", "WARN", etc.
    LogTimeRange time_range{LogTimeRange::kAll};
};

struct FilteredLogResult
{
    std::vector<std::string> lines;
    int total_matches{0};
    std::vector<size_t> match_positions; ///< Positions of matches in each line
};

class LogFilterService
{
public:
    /// Filter log lines by criteria.
    [[nodiscard]] static auto filter(const std::vector<std::string>& log_lines,
                                     const LogFilterCriteria& criteria) -> FilteredLogResult;
};

// ════════════════════════════════════════════════════════════════════
// External Log Shipper (PRD Task 16)
// ════════════════════════════════════════════════════════════════════

enum class LogShipTarget : uint8_t
{
    kFile,
    kSyslog,
    kHttp,
};

struct LogShipConfig
{
    LogShipTarget target{LogShipTarget::kFile};
    std::string endpoint;          ///< File path or HTTP URL
    std::string min_level{"WARN"}; ///< Only ship WARN and above by default
    bool enabled{false};
    size_t buffer_size{1000}; ///< Backpressure buffer
};

class ExternalLogShipper
{
public:
    /// Configure the shipper.
    void configure(const LogShipConfig& config);

    /// Get current configuration.
    [[nodiscard]] auto config() const -> LogShipConfig;

    /// Ship a log entry (stub — buffered in memory for now).
    void ship(const std::string& log_json);

    /// Get buffered entries count.
    [[nodiscard]] auto buffered_count() const -> size_t;

    /// Flush the buffer (stub).
    void flush();

    /// Check if shipping is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

private:
    LogShipConfig config_;
    std::vector<std::string> buffer_;
};

// ════════════════════════════════════════════════════════════════════
// Observability Command Provider (PRD Task 18)
// ════════════════════════════════════════════════════════════════════

struct ObservabilityCommand
{
    std::string command_id;
    std::string label;
    std::string category;
};

class ObservabilityCommandProvider
{
public:
    /// Get all observability commands.
    [[nodiscard]] static auto commands() -> std::vector<ObservabilityCommand>;
};

// ════════════════════════════════════════════════════════════════════
// Observability Settings (PRD Task 19)
// ════════════════════════════════════════════════════════════════════

struct ObservabilitySettings
{
    std::string global_log_level{"INFO"};
    int log_retention_days{7};
    bool metrics_enabled{true};
    int health_check_interval_seconds{30};
    bool auto_diagnostic_on_crash{true};
};

class ObservabilitySettingsManager
{
public:
    /// Get current settings.
    [[nodiscard]] auto settings() const -> ObservabilitySettings;

    /// Update settings.
    void update(const ObservabilitySettings& new_settings);

    /// Reset to defaults.
    void reset_defaults();

private:
    ObservabilitySettings settings_;
};

} // namespace markamp::core
