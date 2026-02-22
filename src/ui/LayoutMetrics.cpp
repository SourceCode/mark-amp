#include "LayoutMetrics.h"

namespace markamp::ui
{

auto LayoutMetrics::get() -> LayoutMetrics&
{
    static LayoutMetrics instance;
    return instance;
}

void LayoutMetrics::set_profile(DensityProfile profile)
{
    profile_ = profile;
    spacing_grid_.set_density(profile);
    ComponentSizeResolver::get().set_density(profile);
}

// ── Row Heights ─────────────────────────────────────────────────────────────

auto LayoutMetrics::row_height() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kTreeRow).height;
}

auto LayoutMetrics::tab_height() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kTab).height;
}

auto LayoutMetrics::toolbar_height() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kToolbarButton).height;
}

auto LayoutMetrics::activity_bar_slot_height() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kActivityBarSlot).height;
}

auto LayoutMetrics::status_bar_height() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kStatusBarSegment).height;
}

// ── Spacing ─────────────────────────────────────────────────────────────────

auto LayoutMetrics::control_padding_h() const -> int
{
    return spacing_grid_.scaled(SpacingToken::kSm);
}

auto LayoutMetrics::control_padding_v() const -> int
{
    return spacing_grid_.scaled(SpacingToken::kXs);
}

auto LayoutMetrics::control_gap() const -> int
{
    return spacing_grid_.scaled(SpacingToken::kXs);
}

auto LayoutMetrics::section_spacing() const -> int
{
    // The previous implementation for comfortable was 20, default 14, compact 8
    // Since kLg is 16 and kMd is 12, the scaling fits best with some custom logic
    // but we can just use the spacing grid. Let's return section_gap() which is scaled kXl (24)
    // wait, let's use kLg scaled roughly or just explicitly map. The instructions say "delegate to
    // SpacingGrid". I'll define reasonable ones:
    if (profile_ == DensityProfile::kComfortable)
        return spacing_grid_.resolve(SpacingToken::kXl);
    if (profile_ == DensityProfile::kCompact)
        return spacing_grid_.resolve(SpacingToken::kSm);
    return spacing_grid_.resolve(SpacingToken::kLg); // default is 16 (close to 14)
}

// ── Icon Sizes ──────────────────────────────────────────────────────────────

auto LayoutMetrics::icon_size() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kTreeRow).icon_size;
}

auto LayoutMetrics::icon_size_large() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kActivityBarSlot).icon_size;
}

auto LayoutMetrics::icon_size_small() const -> int
{
    return ComponentSizeResolver::get().resolve(ComponentKind::kStatusBarSegment).icon_size;
}

// ── Hit Targets ─────────────────────────────────────────────────────────────

auto LayoutMetrics::min_hit_target() const -> int
{
    // Accessibility: always ≥ 24px, even in compact mode
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 32;
        case DensityProfile::kDefault:
            return 28;
        case DensityProfile::kCompact:
            return 24;
    }
    return 28;
}

auto LayoutMetrics::splitter_hit_width() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 12;
        case DensityProfile::kDefault:
            return 8;
        case DensityProfile::kCompact:
            return 6;
    }
    return 8;
}

auto LayoutMetrics::splitter_visual_width() const -> int
{
    return profile_ == DensityProfile::kComfortable ? 2 : 1;
}

// ── Elevation ───────────────────────────────────────────────────────────────

auto LayoutMetrics::focus_ring_width() const -> int
{
    return 2; // constant across densities
}

auto LayoutMetrics::border_width() const -> int
{
    return 1; // constant across densities
}

} // namespace markamp::ui
