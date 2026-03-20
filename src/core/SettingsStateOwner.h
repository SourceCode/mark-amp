/// @file SettingsStateOwner.h
/// @brief P06-T04: Canonical settings state owner for all entry points.
///
/// Provides staged changes, dirty tracking, apply/cancel semantics shared
/// by the settings sidebar, dialog, and JSON editor.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;

/// A single staged setting change.
struct StagedChange
{
    std::string key;
    std::string old_value;
    std::string new_value;
};

/// Canonical state owner for settings editing surfaces.
class SettingsStateOwner
{
public:
    SettingsStateOwner(EventBus& bus, Config& cfg);

    /// Stage a setting change (does not apply yet).
    void stage(const std::string& key, const std::string& value);

    /// Apply all staged changes.
    void apply();

    /// Cancel all staged changes.
    void cancel();

    /// Check if there are staged (unapplied) changes.
    [[nodiscard]] auto is_dirty() const -> bool { return !staged_.empty(); }

    /// Get staged changes.
    [[nodiscard]] auto staged_changes() const -> const std::vector<StagedChange>&
    {
        return staged_;
    }

    /// Get count of staged changes.
    [[nodiscard]] auto staged_count() const -> int
    {
        return static_cast<int>(staged_.size());
    }

    /// Undo the last staged change.
    void undo_last();

    /// Import settings from a map.
    void import_settings(const std::unordered_map<std::string, std::string>& settings);

    /// Export current settings.
    [[nodiscard]] auto export_settings() const
        -> std::unordered_map<std::string, std::string>;

private:
    EventBus& event_bus_;
    Config& config_;
    std::vector<StagedChange> staged_;
};

} // namespace markamp::core
