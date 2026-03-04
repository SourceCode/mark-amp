#pragma once

/**
 * @file CompactLayoutModel.h
 * @brief Phase 36 Task 2: Compact width behavior and platform conventions.
 *
 * Manages width breakpoints (compact/normal/wide), overflow/truncation
 * rules, and platform-specific control order conventions.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Layout width mode.
enum class LayoutWidthMode : uint8_t
{
    kCompact, ///< < 640px — collapse optional, truncate labels
    kNormal,  ///< 640-1024px — standard layout
    kWide,    ///< > 1024px — full controls, expanded panels
};

/// Overflow behavior for narrow layouts.
enum class OverflowBehavior : uint8_t
{
    kTruncate, ///< Truncate text with ellipsis
    kCollapse, ///< Hide entirely
    kOverflow, ///< Show in overflow menu
    kWrap,     ///< Wrap to next line
};

/// A control cluster's layout rules.
struct LayoutRule
{
    std::string cluster_id;
    OverflowBehavior compact_behavior{OverflowBehavior::kCollapse};
    int min_width_px{0};
    int priority{0}; ///< Lower = hide first in overflow

    /// Get overflow behavior name.
    [[nodiscard]] auto behavior_name() const -> std::string;
};

/**
 * @brief Manages compact width behavior and platform conventions.
 */
class CompactLayoutModel
{
public:
    CompactLayoutModel() = default;

    // ── Width mode ─────────────────────────────────────────────────

    /// Determine the mode for a given window width.
    [[nodiscard]] static auto mode_for_width(int width_px) -> LayoutWidthMode;

    /// Get mode name.
    [[nodiscard]] static auto mode_name(LayoutWidthMode mode) -> std::string;

    /// Set the current window width.
    void set_width(int width_px);

    /// Get the current mode.
    [[nodiscard]] auto current_mode() const -> LayoutWidthMode;

    /// Get the current width.
    [[nodiscard]] auto current_width() const -> int;

    // ── Breakpoints ────────────────────────────────────────────────

    /// Set compact breakpoint (below = compact).
    void set_compact_breakpoint(int width_px);

    /// Set wide breakpoint (above = wide).
    void set_wide_breakpoint(int width_px);

    [[nodiscard]] auto compact_breakpoint() const -> int;
    [[nodiscard]] auto wide_breakpoint() const -> int;

    // ── Layout rules ───────────────────────────────────────────────

    /// Register a layout rule for a control cluster.
    void add_rule(const LayoutRule& rule);

    /// Get all registered rules.
    [[nodiscard]] auto all_rules() const -> const std::vector<LayoutRule>&;

    /// Get rules sorted by priority (lowest first = hide first).
    [[nodiscard]] auto rules_by_priority() const -> std::vector<LayoutRule>;

    /// Get visible clusters for the current mode.
    [[nodiscard]] auto visible_clusters() const -> std::vector<std::string>;

    /// Get the overflow behavior for a cluster in the current mode.
    [[nodiscard]] auto behavior_for(const std::string& cluster_id) const -> OverflowBehavior;

    /// Get the number of registered rules.
    [[nodiscard]] auto rule_count() const -> int;

private:
    int current_width_{800};
    int compact_breakpoint_{640};
    int wide_breakpoint_{1024};
    std::vector<LayoutRule> rules_;
};

} // namespace markamp::ui
