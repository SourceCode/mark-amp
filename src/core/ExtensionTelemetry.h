#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Per-extension runtime telemetry data (#41).
struct ExtensionTelemetryData
{
    std::string extension_id;
    std::chrono::milliseconds activation_time{0};
    int api_call_count{0};
    int error_count{0};
    int command_execution_count{0};
    std::chrono::steady_clock::time_point last_active;
};

/// Extension runtime telemetry service (#41).
/// Tracks per-extension activation time, API call counts, error counts.
class ExtensionTelemetry
{
public:
    ExtensionTelemetry() = default;

    /// Record activation time for an extension.
    void record_activation(const std::string& extension_id, std::chrono::milliseconds duration);

    /// Increment API call count.
    void record_api_call(const std::string& extension_id);

    /// Increment error count.
    void record_error(const std::string& extension_id);

    /// Increment command execution count.
    void record_command(const std::string& extension_id);

    /// Get telemetry data for an extension.
    [[nodiscard]] auto get_telemetry(const std::string& extension_id) const
        -> const ExtensionTelemetryData*;

    /// Get all extension IDs with telemetry data.
    [[nodiscard]] auto tracked_extensions() const -> std::vector<std::string>;

    /// Clear telemetry for an extension.
    void clear(const std::string& extension_id);

    /// Clear all telemetry data.
    void clear_all();

private:
    std::unordered_map<std::string, ExtensionTelemetryData> data_;
};

} // namespace markamp::core
