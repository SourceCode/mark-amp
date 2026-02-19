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
}

// ── Row Heights ─────────────────────────────────────────────────────────────

auto LayoutMetrics::row_height() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 32;
        case DensityProfile::kDefault:
            return 26;
        case DensityProfile::kCompact:
            return 22;
    }
    return 26; // unreachable but silences warning
}

auto LayoutMetrics::tab_height() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 38;
        case DensityProfile::kDefault:
            return 32;
        case DensityProfile::kCompact:
            return 28;
    }
    return 32;
}

auto LayoutMetrics::toolbar_height() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 36;
        case DensityProfile::kDefault:
            return 30;
        case DensityProfile::kCompact:
            return 26;
    }
    return 30;
}

auto LayoutMetrics::activity_bar_slot_height() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 52;
        case DensityProfile::kDefault:
            return 44;
        case DensityProfile::kCompact:
            return 38;
    }
    return 44;
}

auto LayoutMetrics::status_bar_height() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 28;
        case DensityProfile::kDefault:
            return 24;
        case DensityProfile::kCompact:
            return 20;
    }
    return 24;
}

// ── Spacing ─────────────────────────────────────────────────────────────────

auto LayoutMetrics::control_padding_h() const -> int
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

auto LayoutMetrics::control_padding_v() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 8;
        case DensityProfile::kDefault:
            return 4;
        case DensityProfile::kCompact:
            return 2;
    }
    return 4;
}

auto LayoutMetrics::control_gap() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 8;
        case DensityProfile::kDefault:
            return 4;
        case DensityProfile::kCompact:
            return 2;
    }
    return 4;
}

auto LayoutMetrics::section_spacing() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 20;
        case DensityProfile::kDefault:
            return 14;
        case DensityProfile::kCompact:
            return 8;
    }
    return 14;
}

// ── Icon Sizes ──────────────────────────────────────────────────────────────

auto LayoutMetrics::icon_size() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 20;
        case DensityProfile::kDefault:
            return 16;
        case DensityProfile::kCompact:
            return 14;
    }
    return 16;
}

auto LayoutMetrics::icon_size_large() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 28;
        case DensityProfile::kDefault:
            return 24;
        case DensityProfile::kCompact:
            return 20;
    }
    return 24;
}

auto LayoutMetrics::icon_size_small() const -> int
{
    switch (profile_)
    {
        case DensityProfile::kComfortable:
            return 16;
        case DensityProfile::kDefault:
            return 12;
        case DensityProfile::kCompact:
            return 10;
    }
    return 12;
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
