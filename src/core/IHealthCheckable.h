/// IHealthCheckable.h — Phase 01: Service health check protocol
///
/// Defines a standard interface for services to report health status.
/// Used by HealthPanel to display system-wide health dashboard.

#pragma once

#include <string>
#include <string_view>

namespace markamp::core
{

/// Health status levels for service reporting.
enum class HealthStatus : uint8_t
{
    kOk,       ///< Service is operating normally
    kDegraded, ///< Service is functional but experiencing issues
    kFailed,   ///< Service has failed and is non-functional
};

/// Standard interface for services that can report health status.
/// Implement on core services (EventBus, Config, ThemeEngine, PluginManager).
class IHealthCheckable
{
public:
    virtual ~IHealthCheckable() = default;
    IHealthCheckable(const IHealthCheckable&) = default;
    auto operator=(const IHealthCheckable&) -> IHealthCheckable& = default;
    IHealthCheckable(IHealthCheckable&&) = default;
    auto operator=(IHealthCheckable&&) -> IHealthCheckable& = default;

    /// Get current health status of this service.
    [[nodiscard]] virtual auto health_status() const -> HealthStatus = 0;

    /// Get human-readable diagnostic info for debugging.
    /// Should include relevant metrics and state information.
    [[nodiscard]] virtual auto diagnostic_info() const -> std::string = 0;

    /// Get the service name for reporting purposes.
    [[nodiscard]] virtual auto service_name() const -> std::string_view = 0;
};

/// Convert HealthStatus to a displayable string.
[[nodiscard]] inline auto health_status_string(HealthStatus status) -> std::string_view
{
    switch (status)
    {
        case HealthStatus::kOk:
            return "ok";
        case HealthStatus::kDegraded:
            return "degraded";
        case HealthStatus::kFailed:
            return "failed";
    }
    return "unknown";
}

} // namespace markamp::core
