#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Overlay anchor position (Phase 23 Task 1).
enum class OverlayAnchor : uint8_t
{
    kAboveCursor,
    kBelowCursor,
    kAboveSelection,
    kBelowSelection,
};

/// An overlay action button.
struct OverlayAction
{
    std::string action_id;
    std::string label;
    std::string shortcut;
    std::string group;     ///< Action group for ordering
    bool is_active{false}; ///< Toggle state (e.g., bold active)
    bool is_enabled{true}; ///< Whether action is available
};

/// Viewport bounds for overlay positioning.
struct ViewportBounds
{
    int width{0};
    int height{0};
};

/// Testable model for Editor Inline Overlays (Phase 23).
///
/// Encapsulates:
/// - Overlay positioning with viewport clamping
/// - Focus trap and escape semantics
/// - Action grouping and state
/// - Visibility and anchor management
class EditorOverlayModel
{
public:
    /// Set overlay actions.
    void set_actions(std::vector<OverlayAction> actions);

    /// Get all actions.
    [[nodiscard]] auto actions() const -> const std::vector<OverlayAction>&;

    /// Get actions by group.
    [[nodiscard]] auto actions_by_group(const std::string& group) const
        -> std::vector<OverlayAction>;

    /// Get unique action groups (sorted).
    [[nodiscard]] auto groups() const -> std::vector<std::string>;

    // ── Positioning ─────────────────────────────────────────────────

    /// Compute clamped position: returns {x, y} after viewport clamping.
    struct Position
    {
        int pos_x;
        int pos_y;
    };
    [[nodiscard]] static auto clamped_position(int desired_x,
                                               int desired_y,
                                               int overlay_w,
                                               int overlay_h,
                                               ViewportBounds viewport) -> Position;

    // ── Anchor ──────────────────────────────────────────────────────

    void set_anchor(OverlayAnchor anchor);
    [[nodiscard]] auto anchor() const -> OverlayAnchor;

    // ── Visibility ──────────────────────────────────────────────────

    void set_visible(bool visible);
    [[nodiscard]] auto is_visible() const -> bool;

    // ── State ───────────────────────────────────────────────────────

    /// Toggle active state of an action.
    void toggle_action(const std::string& action_id);

    /// Set enabled state.
    void set_action_enabled(const std::string& action_id, bool enabled);

private:
    std::vector<OverlayAction> actions_;
    OverlayAnchor anchor_{OverlayAnchor::kAboveCursor};
    bool visible_{false};
};

} // namespace markamp::ui
