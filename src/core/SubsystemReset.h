/// SubsystemReset.h — V7 Phase 37: Registerable per-subsystem reset handlers
///
/// Each subsystem registers a reset handler that returns Result<void>.
/// The reset coordinator can reset individual or all subsystems.

#pragma once

#include "Result.h"

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Subsystem Reset
// ══════════════════════════════════════════════════════════════════════════════

/// Reset handler function type.
using ResetHandler = std::function<Result<void>()>;

/// Result of resetting a subsystem.
struct ResetResult
{
    std::string subsystem_name;
    bool success{false};
    std::string error_message;
};

/// Coordinates per-subsystem reset handlers.
class SubsystemResetCoordinator
{
public:
    SubsystemResetCoordinator() = default;

    /// Register a reset handler for a subsystem.
    void register_handler(const std::string& subsystem_name, ResetHandler handler);

    /// Unregister a reset handler.
    void unregister_handler(const std::string& subsystem_name);

    /// Reset a specific subsystem.
    [[nodiscard]] auto reset_subsystem(const std::string& subsystem_name) -> Result<void>;

    /// Reset all registered subsystems.
    [[nodiscard]] auto reset_all() -> std::vector<ResetResult>;

    /// Check if a subsystem has a registered handler.
    [[nodiscard]] auto has_handler(const std::string& subsystem_name) const -> bool;

    /// Get the list of registered subsystem names.
    [[nodiscard]] auto registered_subsystems() const -> std::vector<std::string>;

    /// Get the number of registered handlers.
    [[nodiscard]] auto handler_count() const -> size_t;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ResetHandler> handlers_;
};

} // namespace markamp::core
