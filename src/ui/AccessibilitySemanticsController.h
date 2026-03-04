#pragma once

/**
 * @file AccessibilitySemanticsController.h
 * @brief Phase 35 Task 1: Accessible names, roles, and states for custom controls.
 *
 * Assigns explicit semantics for custom-rendered widgets, manages
 * high-contrast and reduced-complexity modes, and validates contrast.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// ARIA-like role for custom controls.
enum class AccessibleRole : uint8_t
{
    kNone,
    kButton,
    kCheckbox,
    kRadio,
    kTab,
    kTabPanel,
    kToolbar,
    kMenu,
    kMenuItem,
    kTree,
    kTreeItem,
    kList,
    kListItem,
    kSlider,
    kProgressBar,
    kDialog,
    kAlert,
    kStatus,
    kRegion,
};

/// Accessible state flags.
enum class AccessibleState : uint8_t
{
    kNormal = 0,
    kDisabled = 1 << 0,
    kSelected = 1 << 1,
    kExpanded = 1 << 2,
    kCollapsed = 1 << 3,
    kChecked = 1 << 4,
    kFocused = 1 << 5,
    kHidden = 1 << 6,
};

/// Semantic info for a single control.
struct ControlSemantics
{
    std::string control_id;
    std::string accessible_name;
    std::string accessible_description;
    AccessibleRole role{AccessibleRole::kNone};
    uint8_t state_flags{0};

    /// Check if a state flag is set.
    [[nodiscard]] auto has_state(AccessibleState flag) const -> bool;

    /// Get role as a string.
    [[nodiscard]] auto role_name() const -> std::string;
};

/// Contrast ratio result.
struct ContrastResult
{
    double ratio{0.0};
    bool meets_aa{false};  ///< >= 4.5:1 for normal text
    bool meets_aaa{false}; ///< >= 7:1 for normal text
};

/**
 * @brief Manages accessible semantics for custom controls.
 */
class AccessibilitySemanticsController
{
public:
    AccessibilitySemanticsController() = default;

    // ── Semantic registration ──────────────────────────────────────

    void register_control(const ControlSemantics& semantics);
    void update_state(const std::string& control_id, uint8_t state_flags);
    void remove_control(const std::string& control_id);

    [[nodiscard]] auto semantics_for(const std::string& control_id) const
        -> const ControlSemantics*;
    [[nodiscard]] auto control_count() const -> int;

    // ── Modes ──────────────────────────────────────────────────────

    void set_high_contrast(bool enabled);
    [[nodiscard]] auto is_high_contrast() const -> bool;

    void set_reduced_complexity(bool enabled);
    [[nodiscard]] auto is_reduced_complexity() const -> bool;

    // ── Contrast validation ────────────────────────────────────────

    [[nodiscard]] static auto check_contrast(double luminance_fg, double luminance_bg)
        -> ContrastResult;

    // ── Focus visibility ───────────────────────────────────────────

    void set_focus_visible(bool enabled);
    [[nodiscard]] auto is_focus_visible() const -> bool;

private:
    std::vector<ControlSemantics> controls_;
    bool high_contrast_{false};
    bool reduced_complexity_{false};
    bool focus_visible_{true};
};

} // namespace markamp::ui
