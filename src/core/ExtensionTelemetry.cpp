#include "ExtensionTelemetry.h"

#include <vector>

namespace markamp::core
{

void ExtensionTelemetry::record_activation(const std::string& extension_id,
                                           std::chrono::milliseconds duration)
{
    auto& entry = data_[extension_id];
    entry.extension_id = extension_id;
    entry.activation_time = duration;
    entry.last_active = std::chrono::steady_clock::now();
}

void ExtensionTelemetry::record_api_call(const std::string& extension_id)
{
    auto& entry = data_[extension_id];
    entry.extension_id = extension_id;
    ++entry.api_call_count;
    entry.last_active = std::chrono::steady_clock::now();
}

void ExtensionTelemetry::record_error(const std::string& extension_id)
{
    auto& entry = data_[extension_id];
    entry.extension_id = extension_id;
    ++entry.error_count;
    entry.last_active = std::chrono::steady_clock::now();
}

void ExtensionTelemetry::record_command(const std::string& extension_id)
{
    auto& entry = data_[extension_id];
    entry.extension_id = extension_id;
    ++entry.command_execution_count;
    entry.last_active = std::chrono::steady_clock::now();
}

auto ExtensionTelemetry::get_telemetry(const std::string& extension_id) const
    -> const ExtensionTelemetryData*
{
    auto found = data_.find(extension_id);
    return found != data_.end() ? &found->second : nullptr;
}

auto ExtensionTelemetry::tracked_extensions() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(data_.size());
    for (const auto& [ext_id, telemetry] : data_)
    {
        result.push_back(ext_id);
    }
    return result;
}

void ExtensionTelemetry::clear(const std::string& extension_id)
{
    data_.erase(extension_id);
}

void ExtensionTelemetry::clear_all()
{
    data_.clear();
}

} // namespace markamp::core
