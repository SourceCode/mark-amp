/// @file EntryPointNormalizer.h
/// @brief V20 P06-T03/T05: Entry-point normalization and panel activation rules.
///
/// Ensures every visible affordance for creating artifacts points to the same
/// canonical command workflow. Provides panel activation rules that prevent
/// panels from breaking flow during create/open/save transitions.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Entry point surface type.
enum class EntryPointSurface
{
    kMainMenu,
    kToolbar,
    kPalette,
    kExplorerContextMenu,
    kTabBarButton,
    kWelcomeScreen,
    kEmptyState,
    kShortcut,
    kDragDrop
};

/// Mapping from a surface entry point to its canonical command.
struct EntryPointMapping
{
    EntryPointSurface surface{EntryPointSurface::kMainMenu};
    std::string command_id;        ///< Canonical command to invoke
    std::string label;             ///< Display label at this surface
    bool is_active{true};          ///< Whether this entry point is live

    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return !command_id.empty() && is_active;
    }
};

/// Panel activation rule for workflow integration.
struct PanelActivationRule
{
    std::string panel_id;
    std::string trigger;           ///< "on_create", "on_save", "on_search", etc.
    bool should_activate{false};
    bool should_steal_focus{false};
    std::string focus_return_target; ///< Where to return focus after panel action
};

/// Normalizes all creation entry points and manages panel activation rules.
class EntryPointNormalizer
{
public:
    /// Register an entry-point mapping.
    void register_entry_point(const EntryPointMapping& mapping);

    /// Get mappings for a given surface.
    [[nodiscard]] auto mappings_for_surface(EntryPointSurface surface) const
        -> std::vector<EntryPointMapping>;

    /// Resolve the canonical command for a surface + action pair.
    [[nodiscard]] auto resolve_command(EntryPointSurface surface,
                                        const std::string& action = {}) const
        -> std::string;

    /// Total registrations.
    [[nodiscard]] auto entry_point_count() const noexcept -> int
    {
        return static_cast<int>(mappings_.size());
    }

    // ── Panel Rules ──

    /// Register a panel activation rule.
    void register_panel_rule(const PanelActivationRule& rule);

    /// Get rules for a given trigger event.
    [[nodiscard]] auto rules_for_trigger(const std::string& trigger) const
        -> std::vector<PanelActivationRule>;

    /// Determine focus return target after a panel action.
    [[nodiscard]] auto focus_return_for(const std::string& panel_id) const -> std::string;

    /// Total panel rules.
    [[nodiscard]] auto panel_rule_count() const noexcept -> int
    {
        return static_cast<int>(panel_rules_.size());
    }

private:
    std::vector<EntryPointMapping> mappings_;
    std::vector<PanelActivationRule> panel_rules_;
};

} // namespace markamp::core
