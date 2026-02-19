#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// ARIA-like role for semantic controls.
enum class ControlRole : uint8_t
{
    kButton,
    kCheckbox,
    kRadio,
    kTab,
    kTabPanel,
    kTreeItem,
    kMenuItem,
    kSlider,
    kTextInput,
    kDialog,
    kAlert,
    kStatus,
    kToolbar,
    kListItem,
};

/// Accessibility descriptor for a control.
struct AccessibleControl
{
    std::string control_id;
    std::string accessible_name;
    ControlRole role{ControlRole::kButton};
    std::string state_description; ///< e.g., "checked", "expanded"
    bool is_focusable{true};
};

/// Pending announcement.
struct Announcement
{
    std::string message;
    bool is_assertive{false}; ///< true = assertive/alert, false = polite
};

/// Testable model for Accessibility Semantics (Phase 35).
///
/// Encapsulates:
/// - Control registry with role/name/state
/// - Screen reader announcements (polite/assertive)
/// - High-contrast mode
/// - Reduced-complexity mode
/// - Focus order validation
class AccessibilityModel
{
public:
    // ── Controls ────────────────────────────────────────────────────

    void set_controls(std::vector<AccessibleControl> controls);
    [[nodiscard]] auto controls() const -> const std::vector<AccessibleControl>&;
    [[nodiscard]] auto control_by_id(const std::string& control_id) const
        -> const AccessibleControl*;

    /// All controls missing an accessible name.
    [[nodiscard]] auto unnamed_controls() const -> std::vector<AccessibleControl>;

    /// All non-focusable controls (for audit).
    [[nodiscard]] auto non_focusable() const -> std::vector<AccessibleControl>;

    // ── Announcements ───────────────────────────────────────────────

    void announce(const std::string& message, bool assertive = false);
    [[nodiscard]] auto pending_announcements() const -> const std::vector<Announcement>&;
    void clear_announcements();

    // ── Modes ───────────────────────────────────────────────────────

    void set_high_contrast(bool enabled);
    [[nodiscard]] auto high_contrast() const -> bool;

    void set_reduced_complexity(bool enabled);
    [[nodiscard]] auto reduced_complexity() const -> bool;

private:
    std::vector<AccessibleControl> controls_;
    std::vector<Announcement> announcements_;
    bool high_contrast_{false};
    bool reduced_complexity_{false};
};

} // namespace markamp::ui
