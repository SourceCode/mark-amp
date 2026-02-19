#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// How a toolbar button behaves when clicked.
enum class ToolbarButtonKind : uint8_t
{
    kMomentary, ///< Click fires action, no persistent state
    kToggle,    ///< Click toggles on/off state
};

/// A toolbar button's model state.
struct ToolbarButtonModel
{
    std::string button_id;     ///< Unique action ID
    std::string label;         ///< Display text
    std::string tooltip;       ///< Full tooltip with description
    std::string shortcut_hint; ///< Shortcut (e.g., "Cmd+S")
    ToolbarButtonKind kind{ToolbarButtonKind::kMomentary};
    bool is_toggled{false}; ///< Current toggle state (only meaningful for kToggle)
    bool is_visible{true};  ///< Whether button is in the visible area
    std::string context;    ///< Mode context this button belongs to (e.g., "editor", "canvas")
};

/// Contextual action slot — a group of buttons associated with a workbench mode.
struct ActionSlot
{
    std::string mode; ///< Workbench mode (e.g., "editor", "canvas", "mindmap")
    std::vector<ToolbarButtonModel> buttons; ///< Actions available in this mode
};

/// Testable model for the Toolbar action surface (Phase 09).
///
/// Encapsulates:
/// - Global vs. contextual action slots
/// - Toggle vs. momentary button semantics
/// - Overflow detection
/// - Shortcut and secondary-action hints
class ToolbarModel
{
public:
    /// Register global actions (always visible).
    void set_global_actions(std::vector<ToolbarButtonModel> actions);

    /// Register a contextual action slot.
    void add_context_slot(ActionSlot slot);

    /// Switch the active workbench mode.
    void set_mode(const std::string& mode);

    /// Get the active mode.
    [[nodiscard]] auto active_mode() const -> std::string;

    /// Get the buttons currently visible (global + active context).
    [[nodiscard]] auto visible_buttons() const -> std::vector<ToolbarButtonModel>;

    /// Get global-only buttons.
    [[nodiscard]] auto global_buttons() const -> const std::vector<ToolbarButtonModel>&;

    /// Get context buttons for active mode.
    [[nodiscard]] auto context_buttons() const -> std::vector<ToolbarButtonModel>;

    // ── Toggle semantics ────────────────────────────────────────────

    /// Toggle a button's state (only valid for kToggle buttons). Returns new state.
    auto toggle(const std::string& button_id) -> bool;

    // ── Overflow ────────────────────────────────────────────────────

    /// Given a viewport width and button width, return indices of overflowed buttons.
    [[nodiscard]] auto overflowed_indices(int viewport_width, int button_width) const
        -> std::vector<int>;

    // ── Tooltip generation ──────────────────────────────────────────

    /// Build tooltip text for a button (label + shortcut + toggle state).
    [[nodiscard]] static auto build_tooltip(const ToolbarButtonModel& button) -> std::string;

private:
    std::vector<ToolbarButtonModel> global_actions_;
    std::vector<ActionSlot> context_slots_;
    std::string active_mode_;
};

} // namespace markamp::ui
