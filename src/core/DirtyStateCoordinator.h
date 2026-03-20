/// @file DirtyStateCoordinator.h
/// @brief V19 P03: Surface-agnostic dirty-state aggregation.
///
/// Provides a single source of truth for whether the workbench session has
/// unsaved changes across any surface (editor, notebook, canvas). Supports
/// contributor registration, dirty-state queries, save-all, and close-check
/// flows.
#pragma once

#include "EventBus.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Describes a single dirty-state contributor.
struct DirtyContributor
{
    std::string surface_id;   ///< e.g. "editor:main", "notebook:0", "canvas:board1"
    std::string display_name; ///< Human-readable name for close/save dialogs

    /// Returns true if this contributor has unsaved changes.
    std::function<bool()> is_dirty_fn;

    /// Attempts to save this contributor's changes. Returns true on success.
    std::function<bool()> save_fn;
};

/// Aggregates dirty state across all workbench surfaces.
///
/// Usage:
/// ```cpp
/// DirtyStateCoordinator coordinator(event_bus);
/// coordinator.register_contributor({
///     "editor:main", "Untitled.md",
///     [&]() { return editor.is_modified(); },
///     [&]() { return editor.save(); }
/// });
/// if (coordinator.is_session_dirty()) { /* prompt before close */ }
/// ```
class DirtyStateCoordinator
{
public:
    explicit DirtyStateCoordinator(EventBus& event_bus);

    /// Register a dirty-state contributor.
    void register_contributor(DirtyContributor contributor);

    /// Unregister a contributor by surface ID.
    auto unregister_contributor(const std::string& surface_id) -> bool;

    /// Check if any contributor has unsaved changes.
    [[nodiscard]] auto is_session_dirty() const -> bool;

    /// Get the list of dirty contributors (surfaces with unsaved changes).
    [[nodiscard]] auto get_dirty_contributors() const -> std::vector<std::string>;

    /// Save all dirty contributors. Returns number of failures.
    auto save_all() -> int;

    /// Close check: returns true if safe to close (all saved or user consented).
    /// In production, this shows a prompt; in test, the close policy callback is used.
    [[nodiscard]] auto close_check() -> bool;

    /// Set a close policy callback (for testing without UI).
    /// Returns true to consent to data loss, false to cancel close.
    using ClosePolicyFn = std::function<bool(const std::vector<std::string>& dirty_surfaces)>;
    void set_close_policy(ClosePolicyFn policy);

    /// Total registered contributor count.
    [[nodiscard]] auto contributor_count() const -> int
    {
        return static_cast<int>(contributors_.size());
    }

private:
    [[maybe_unused]] EventBus& event_bus_;
    std::unordered_map<std::string, DirtyContributor> contributors_;
    ClosePolicyFn close_policy_;
};

} // namespace markamp::core
