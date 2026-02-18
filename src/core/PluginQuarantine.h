/// PluginQuarantine.h — V7 Phase 25: Plugin fault quarantine
///
/// Manages a persistent quarantine list for plugins that have repeatedly
/// crashed or misbehaved. Quarantined plugins are skipped during extension
/// host restart.

#pragma once

#include "Result.h"

#include <chrono>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Quarantine Entry
// ══════════════════════════════════════════════════════════════════════════════

/// Describes why a plugin was quarantined.
struct QuarantineEntry
{
    std::string plugin_id;
    std::string reason;
    int64_t quarantined_at_ms{0}; // Unix timestamp milliseconds
    int crash_count{0};
    bool manual{false}; // true if manually quarantined by user
};

// ══════════════════════════════════════════════════════════════════════════════
// Plugin Quarantine
// ══════════════════════════════════════════════════════════════════════════════

/// Manages quarantined plugins with optional persistence.
class PluginQuarantine
{
public:
    PluginQuarantine() = default;

    /// Set the file path for persistent quarantine storage.
    void set_persistence_path(const std::filesystem::path& path);

    /// Quarantine a plugin.
    void quarantine(const std::string& plugin_id,
                    const std::string& reason,
                    int crash_count = 0,
                    bool manual = false);

    /// Remove a plugin from quarantine.
    void unquarantine(const std::string& plugin_id);

    /// Check if a plugin is quarantined.
    [[nodiscard]] auto is_quarantined(const std::string& plugin_id) const -> bool;

    /// Get quarantine entry for a plugin (if quarantined).
    [[nodiscard]] auto get_entry(const std::string& plugin_id) const
        -> std::optional<QuarantineEntry>;

    /// Get all quarantine entries.
    [[nodiscard]] auto all_entries() const -> std::vector<QuarantineEntry>;

    /// Record a crash for a plugin. Auto-quarantine if threshold exceeded.
    /// Returns true if the plugin was auto-quarantined.
    auto record_crash(const std::string& plugin_id, int auto_quarantine_threshold = 3) -> bool;

    /// Get crash count for a plugin (even if not yet quarantined).
    [[nodiscard]] auto crash_count(const std::string& plugin_id) const -> int;

    /// Clear all quarantine entries.
    void clear();

    /// Save quarantine state to the persistence path.
    [[nodiscard]] auto save() const -> Result<void>;

    /// Load quarantine state from the persistence path.
    [[nodiscard]] auto load() -> Result<void>;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, QuarantineEntry> entries_;
    std::unordered_map<std::string, int> crash_counts_;
    std::filesystem::path persistence_path_;
};

} // namespace markamp::core
