#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// A quick action on the startup panel (Phase 20 Task 1).
struct StartupAction
{
    std::string action_id;
    std::string label;
    std::string shortcut;  ///< Keyboard shortcut
    std::string icon_hint; ///< Icon identifier
    int priority{0};       ///< Lower = higher priority
};

/// Workspace intent preset (Phase 20 Task 2).
struct WorkspacePreset
{
    std::string preset_id;
    std::string label;
    std::string description;
    std::string layout_mode; ///< e.g., "writing", "canvas", "review"
};

/// A recent workspace entry (Phase 20 Task 3).
struct RecentWorkspace
{
    std::string workspace_path;
    std::string display_name;
    bool is_pinned{false};
    int open_count{0};
};

/// Testable model for Startup/Onboarding (Phase 20).
///
/// Encapsulates:
/// - Quick action ordering
/// - Workspace preset selection
/// - Recent workspaces (pin, remove, search, sort)
/// - First-run detection
class StartupModel
{
public:
    /// Set available quick actions.
    void set_actions(std::vector<StartupAction> actions);

    /// Get actions sorted by priority.
    [[nodiscard]] auto sorted_actions() const -> std::vector<StartupAction>;

    // ── Presets ─────────────────────────────────────────────────────

    /// Set available presets.
    void set_presets(std::vector<WorkspacePreset> presets);

    /// Get all presets.
    [[nodiscard]] auto presets() const -> const std::vector<WorkspacePreset>&;

    /// Get preset by ID.
    [[nodiscard]] auto preset_by_id(const std::string& preset_id) const -> const WorkspacePreset*;

    // ── Recents ─────────────────────────────────────────────────────

    /// Set recent workspaces.
    void set_recents(std::vector<RecentWorkspace> recents);

    /// Get recents sorted: pinned first, then by open_count descending.
    [[nodiscard]] auto sorted_recents() const -> std::vector<RecentWorkspace>;

    /// Pin/unpin a recent workspace.
    void toggle_pin(const std::string& workspace_path);

    /// Remove a recent workspace.
    void remove_recent(const std::string& workspace_path);

    /// Search recents by name.
    [[nodiscard]] auto search_recents(const std::string& query) const
        -> std::vector<RecentWorkspace>;

    /// Recent count.
    [[nodiscard]] auto recent_count() const -> int;

    // ── First-run ───────────────────────────────────────────────────

    /// Set/get first-run flag.
    void set_first_run(bool is_first);
    [[nodiscard]] auto is_first_run() const -> bool;

private:
    std::vector<StartupAction> actions_;
    std::vector<WorkspacePreset> presets_;
    std::vector<RecentWorkspace> recents_;
    bool first_run_{false};
};

} // namespace markamp::ui
