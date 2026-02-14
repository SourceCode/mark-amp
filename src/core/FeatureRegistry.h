#pragma once

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Metadata for a toggleable feature registered in the system.
struct FeatureInfo
{
    std::string id;           // Unique feature key, e.g. "mermaid"
    std::string display_name; // Human-readable name, e.g. "Mermaid Diagrams"
    std::string description;  // Help text
    bool default_enabled{true};
};

/// Central registry for toggleable application features.
///
/// Features are identified by a unique string ID and backed by Config
/// keys of the form "feature.<id>.enabled". When a feature's enabled
/// state changes, a FeatureToggledEvent is published via EventBus.
///
/// Thread-safe for reads; write operations (enable/disable/toggle)
/// must be called from the main UI thread (fires events).
class FeatureRegistry
{
public:
    FeatureRegistry(EventBus& event_bus, Config& config);

    /// Register a new toggleable feature. Reads initial state from Config
    /// (or applies default_enabled if no persisted value exists).
    void register_feature(const FeatureInfo& info);

    /// Query whether a feature is currently enabled.
    /// Returns false for unknown feature IDs.
    [[nodiscard]] auto is_enabled(const std::string& feature_id) const -> bool;

    /// Enable a feature. No-op if already enabled or unknown.
    void enable(const std::string& feature_id);

    /// Disable a feature. No-op if already disabled or unknown.
    void disable(const std::string& feature_id);

    /// Toggle a feature's enabled state. No-op for unknown IDs.
    void toggle(const std::string& feature_id);

    /// Set a feature's enabled state explicitly.
    void set_enabled(const std::string& feature_id, bool enabled);

    /// Get all registered feature infos.
    [[nodiscard]] auto get_all_features() const -> std::vector<FeatureInfo>;

    /// Get a single feature's info. Returns nullptr if not found.
    [[nodiscard]] auto get_feature(const std::string& feature_id) const -> const FeatureInfo*;

    /// Number of registered features.
    [[nodiscard]] auto feature_count() const -> std::size_t;

private:
    EventBus& event_bus_;
    Config& config_;

    struct FeatureEntry
    {
        FeatureInfo info;
        bool enabled{true};
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, FeatureEntry> features_;

    /// Config key for a feature's enabled state.
    [[nodiscard]] static auto config_key(const std::string& feature_id) -> std::string;
};

} // namespace markamp::core
