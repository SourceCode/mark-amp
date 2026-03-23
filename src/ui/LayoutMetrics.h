#pragma once

#include "ComponentSizeResolver.h"
#include "DensityProfile.h"
#include "SpacingGrid.h"
#include "core/VisualLanguageTokens.h"

#include <cstdint>

namespace markamp::ui
{

/// Centralized layout metrics that all chrome controls consume.
///
/// Usage: `const auto& m = LayoutMetrics::get();`
/// Access: `m.row_height()`, `m.icon_size()`, `m.control_padding()`, etc.
///
/// All metric values are in logical pixels (DPI-independent).
class LayoutMetrics
{
public:
    /// Singleton accessor.
    static auto get() -> LayoutMetrics&;

    /// Set the active density profile. All dependent controls should
    /// re-query metrics after this changes.
    void set_profile(DensityProfile profile);

    /// Current density profile.
    [[nodiscard]] auto profile() const -> DensityProfile
    {
        return profile_;
    }

    // ── Row Heights ─────────────────────────────────────────────────────

    /// Standard row height for tree items, list rows, settings entries.
    [[nodiscard]] auto row_height() const -> int;

    /// Tab bar strip height.
    [[nodiscard]] auto tab_height() const -> int;

    /// Toolbar button height (square).
    [[nodiscard]] auto toolbar_height() const -> int;

    /// Activity bar icon slot height.
    [[nodiscard]] auto activity_bar_slot_height() const -> int;

    /// Status bar height.
    [[nodiscard]] auto status_bar_height() const -> int;

    // ── Spacing ─────────────────────────────────────────────────────────

    /// Inside-control content padding (horizontal).
    [[nodiscard]] auto control_padding_h() const -> int;

    /// Inside-control content padding (vertical).
    [[nodiscard]] auto control_padding_v() const -> int;

    /// Gap between adjacent controls in a toolbar/status bar.
    [[nodiscard]] auto control_gap() const -> int;

    /// Section spacing (between groups in settings, panels, etc.).
    [[nodiscard]] auto section_spacing() const -> int;

    // ── Icon Sizes ──────────────────────────────────────────────────────

    /// Standard icon size for tree items, list items.
    [[nodiscard]] auto icon_size() const -> int;

    /// Large icon size for activity bar icons.
    [[nodiscard]] auto icon_size_large() const -> int;

    /// Small icon size for breadcrumb, status bar.
    [[nodiscard]] auto icon_size_small() const -> int;

    // ── Hit Targets ─────────────────────────────────────────────────────

    /// Minimum hit-target dimension (accessibility: ≥ 24px even in compact).
    [[nodiscard]] auto min_hit_target() const -> int;

    /// Drag hit target width for splitters (sidebar, panels).
    [[nodiscard]] auto splitter_hit_width() const -> int;

    /// Visual line width for splitters.
    [[nodiscard]] auto splitter_visual_width() const -> int;

    // ── Elevation ───────────────────────────────────────────────────────

    /// Focus ring width in pixels.
    [[nodiscard]] auto focus_ring_width() const -> int;

    /// Standard border width.
    [[nodiscard]] auto border_width() const -> int;

    // ── V22 Phase 01: Extended Geometry ─────────────────────────────────

    /// Corner radius for a given V22 token.
    [[nodiscard]] static auto corner_radius(core::CornerRadiusToken token) -> int
    {
        return core::resolve_corner_radius(token);
    }

    /// Border weight for a given V22 token.
    [[nodiscard]] static auto border_weight(core::BorderWeightToken token) -> int
    {
        return core::resolve_border_weight(token);
    }

    /// Tree-specific row height.
    [[nodiscard]] auto row_height_tree() const -> int;

    /// List-specific row height.
    [[nodiscard]] auto row_height_list() const -> int;

    /// Standardized panel header height.
    [[nodiscard]] auto panel_header_height() const -> int;

    /// Breadcrumb bar height.
    [[nodiscard]] auto breadcrumb_height() const -> int;

    // ── V26 Phase 01: Premium Layout Metrics ────────────────────────────

    /// Standard dialog padding (horizontal).
    [[nodiscard]] auto dialog_padding_h() const -> int;

    /// Standard dialog padding (vertical).
    [[nodiscard]] auto dialog_padding_v() const -> int;

    /// Panel header height.
    [[nodiscard]] auto panel_header_height_v26() const -> int;

    /// Settings category gap.
    [[nodiscard]] auto settings_category_gap() const -> int;

    /// Settings group gap.
    [[nodiscard]] auto settings_group_gap() const -> int;

    /// Settings row minimum height.
    [[nodiscard]] auto settings_row_min_height() const -> int;

private:
    LayoutMetrics() = default;
    DensityProfile profile_{DensityProfile::kDefault};
    SpacingGrid spacing_grid_;
};

} // namespace markamp::ui
