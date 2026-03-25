/// @file ObservabilityCommands.cpp
/// @brief V9 Phase 33 – User-facing observability implementation.

#include "ObservabilityCommands.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════════════
// Module name extraction
// ════════════════════════════════════════════════════════════════════

auto extract_module_name(const char* file_path) -> std::string
{
    std::string path(file_path);
    // Strip directory and extension to get module name
    auto last_sep = path.find_last_of("/\\");
    if (last_sep != std::string::npos)
    {
        path = path.substr(last_sep + 1);
    }
    auto dot = path.find_last_of('.');
    if (dot != std::string::npos)
    {
        path = path.substr(0, dot);
    }
    // Convert to lowercase
    std::transform(path.begin(),
                   path.end(),
                   path.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
    return path;
}

// ════════════════════════════════════════════════════════════════════
// LogChannels
// ════════════════════════════════════════════════════════════════════

auto LogChannels::all() -> std::vector<std::string>
{
    return {
        kGeneral,
        kEditor,
        kSync,
        kPerformance,
        kSecurity,
        kExtensions,
        kActivity,
        kHealth,
    };
}

// ════════════════════════════════════════════════════════════════════
// UserActionLogger
// ════════════════════════════════════════════════════════════════════

void UserActionLogger::log_action(UserActionType type, const std::string& action_name)
{
    if (!enabled_)
    {
        return;
    }

    UserAction action;
    action.type = type;
    action.action_name = action_name;
    action.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::system_clock::now().time_since_epoch())
                              .count();
    actions_.push_back(action);
}

void UserActionLogger::set_enabled(bool enabled)
{
    enabled_ = enabled;
}

auto UserActionLogger::is_enabled() const -> bool
{
    return enabled_;
}

auto UserActionLogger::recent_actions(size_t count) const -> std::vector<UserAction>
{
    if (actions_.size() <= count)
    {
        return actions_;
    }
    return std::vector<UserAction>(actions_.end() - static_cast<ptrdiff_t>(count), actions_.end());
}

void UserActionLogger::clear()
{
    actions_.clear();
}

auto UserActionLogger::export_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"actions\":[";
    bool first = true;
    for (const auto& action : actions_)
    {
        if (!first)
        {
            oss << ',';
        }
        first = false;

        const char* type_str = "unknown";
        switch (action.type)
        {
            case UserActionType::kCommand:
                type_str = "command";
                break;
            case UserActionType::kFileOpen:
                type_str = "file_open";
                break;
            case UserActionType::kSurfaceTransition:
                type_str = "surface_transition";
                break;
            case UserActionType::kSearch:
                type_str = "search";
                break;
            case UserActionType::kFileSave:
                type_str = "file_save";
                break;
        }

        oss << "{\"type\":\"" << type_str << "\",\"name\":\"" << action.action_name
            << "\",\"ts\":" << action.timestamp_ms << '}';
    }
    oss << "]}";
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════
// LogFilterService
// ════════════════════════════════════════════════════════════════════

auto LogFilterService::filter(const std::vector<std::string>& log_lines,
                              const LogFilterCriteria& criteria) -> FilteredLogResult
{
    FilteredLogResult result;

    for (const auto& line : log_lines)
    {
        bool passes = true;

        // Level filter
        if (!criteria.level_filter.empty())
        {
            if (line.find("[" + criteria.level_filter + "]") == std::string::npos)
            {
                passes = false;
            }
        }

        // Search text filter
        if (passes && !criteria.search_text.empty())
        {
            auto pos = line.find(criteria.search_text);
            if (pos == std::string::npos)
            {
                passes = false;
            }
            else
            {
                result.match_positions.push_back(pos);
            }
        }

        if (passes)
        {
            result.lines.push_back(line);
            result.total_matches++;
        }
    }
    return result;
}

// ════════════════════════════════════════════════════════════════════
// ExternalLogShipper
// ════════════════════════════════════════════════════════════════════

void ExternalLogShipper::configure(const LogShipConfig& ship_config)
{
    config_ = ship_config;
}

auto ExternalLogShipper::config() const -> LogShipConfig
{
    return config_;
}

void ExternalLogShipper::ship(const std::string& log_json)
{
    if (!config_.enabled)
    {
        return;
    }
    buffer_.push_back(log_json);

    // Enforce buffer limit
    if (buffer_.size() > config_.buffer_size)
    {
        buffer_.erase(buffer_.begin());
    }
}

auto ExternalLogShipper::buffered_count() const -> size_t
{
    return buffer_.size();
}

void ExternalLogShipper::flush()
{
    // In production, this would send buffered entries to the target.
    // For now, just clear the buffer.
    buffer_.clear();
}

auto ExternalLogShipper::is_enabled() const -> bool
{
    return config_.enabled;
}

// ════════════════════════════════════════════════════════════════════
// ObservabilityCommandProvider
// ════════════════════════════════════════════════════════════════════

auto ObservabilityCommandProvider::commands() -> std::vector<ObservabilityCommand>
{
    return {
        {"log.show_output", "Log: Show Output", "Log"},
        {"log.set_level", "Log: Set Level", "Log"},
        {"log.show_channel", "Log: Show Channel", "Log"},
        {"metrics.show_dashboard", "Metrics: Show Dashboard", "Metrics"},
        {"health.show_status", "Health: Show Status", "Health"},
        {"diagnostic.generate_report", "Diagnostic: Generate Report", "Diagnostic"},
    };
}

// ════════════════════════════════════════════════════════════════════
// ObservabilitySettingsManager
// ════════════════════════════════════════════════════════════════════

auto ObservabilitySettingsManager::settings() const -> ObservabilitySettings
{
    return settings_;
}

void ObservabilitySettingsManager::update(const ObservabilitySettings& new_settings)
{
    settings_ = new_settings;
}

void ObservabilitySettingsManager::reset_defaults()
{
    settings_ = ObservabilitySettings{};
}

} // namespace markamp::core
