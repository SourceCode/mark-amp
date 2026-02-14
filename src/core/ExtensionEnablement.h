#pragma once

#include "ExtensionManifest.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Service to enable/disable extensions, persisting disabled IDs in Config.
/// Fires `ExtensionEnablementChangedEvent` via EventBus on state changes.
class ExtensionEnablementService
{
public:
    ExtensionEnablementService(EventBus& event_bus, Config& config);

    /// Check if an extension is enabled. Extensions are enabled by default.
    [[nodiscard]] auto is_enabled(const std::string& extension_id) const -> bool;

    /// Enable an extension. Fires ExtensionEnablementChangedEvent.
    void enable(const std::string& extension_id);

    /// Disable an extension. Fires ExtensionEnablementChangedEvent.
    void disable(const std::string& extension_id);

    /// Toggle an extension's enablement. Returns new state.
    auto toggle(const std::string& extension_id) -> bool;

    /// Get all disabled extension IDs.
    [[nodiscard]] auto get_disabled_ids() const -> std::vector<std::string>;

    /// Get count of disabled extensions.
    [[nodiscard]] auto disabled_count() const -> size_t;

private:
    EventBus& event_bus_;
    Config& config_;
    std::unordered_set<std::string> disabled_ids_;

    /// Load disabled IDs from Config.
    void load_from_config();

    /// Save disabled IDs to Config.
    void save_to_config() const;

    /// Config key for persisting disabled extension IDs.
    static constexpr const char* kConfigKey = "extensions.disabled";
};

} // namespace markamp::core
