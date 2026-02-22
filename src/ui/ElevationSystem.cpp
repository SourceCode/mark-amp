#include "ElevationSystem.h"

#include <wx/brush.h>
#include <wx/pen.h>
namespace markamp::ui
{

void ShadowSpec::Render(wxGraphicsContext& gc, const wxRect& bounds) const
{
    if (bounds.IsEmpty() || opacity <= 0.0f)
    {
        return;
    }

    wxRect outer_bounds = bounds;
    outer_bounds.Inflate(blur_radius + spread_radius);
    outer_bounds.Offset(offset_x, offset_y);

    wxColour fill_color = color;
    fill_color.Set(fill_color.Red(),
                   fill_color.Green(),
                   fill_color.Blue(),
                   static_cast<unsigned char>(255 * opacity));

    // A real implementation would use a radial/linear gradient or a specific drop-shadow native
    // API. Here we simulate the shadow by drawing a slightly larger rounded rect with transparency.
    gc.SetBrush(wxBrush(fill_color));
    gc.SetPen(*wxTRANSPARENT_PEN);
    gc.DrawRoundedRectangle(outer_bounds.x,
                            outer_bounds.y,
                            outer_bounds.width,
                            outer_bounds.height,
                            blur_radius > 0 ? blur_radius : 1);
}

ElevationSystem::ElevationSystem()
{
    InitializeShadows();
}

void ElevationSystem::Render(wxGraphicsContext& gc,
                             const wxRect& bounds,
                             ElevationLevel level) const
{
    if (level == ElevationLevel::kNone)
    {
        return;
    }

    size_t idx = static_cast<size_t>(level);
    if (idx >= shadows_light_.size())
    {
        return;
    }

    const auto& shadows = dark_mode_ ? shadows_dark_[idx] : shadows_light_[idx];
    for (const auto& shadow : shadows)
    {
        shadow.Render(gc, bounds);
    }
}

void ElevationSystem::SetDarkMode(bool dark)
{
    dark_mode_ = dark;
}

void ElevationSystem::InitializeShadows()
{
    // kNone
    shadows_light_[static_cast<size_t>(ElevationLevel::kNone)] = {};
    shadows_dark_[static_cast<size_t>(ElevationLevel::kNone)] = {};

    // Base colors for shadows
    wxColour light_shadow(0, 0, 0);
    wxColour dark_glow(255, 255, 255);

    // kLow: 1 layer, 2px blur, 1px y-offset
    shadows_light_[static_cast<size_t>(ElevationLevel::kLow)] = {
        ShadowSpec(0, 1, 2, 0, light_shadow, 0.1f)};
    shadows_dark_[static_cast<size_t>(ElevationLevel::kLow)] = {
        ShadowSpec(0, 1, 2, 0, dark_glow, 0.05f)};

    // kMedium: 2 layers, 4px + 8px blur
    shadows_light_[static_cast<size_t>(ElevationLevel::kMedium)] = {
        ShadowSpec(0, 2, 4, 0, light_shadow, 0.1f), ShadowSpec(0, 4, 8, 0, light_shadow, 0.05f)};
    shadows_dark_[static_cast<size_t>(ElevationLevel::kMedium)] = {
        ShadowSpec(0, 2, 4, 0, dark_glow, 0.05f), ShadowSpec(0, 4, 8, 0, dark_glow, 0.02f)};

    // kHigh: 3 layers, 8px + 16px + 24px blur
    shadows_light_[static_cast<size_t>(ElevationLevel::kHigh)] = {
        ShadowSpec(0, 4, 8, 0, light_shadow, 0.1f),
        ShadowSpec(0, 8, 16, 0, light_shadow, 0.05f),
        ShadowSpec(0, 12, 24, 0, light_shadow, 0.03f)};
    shadows_dark_[static_cast<size_t>(ElevationLevel::kHigh)] = {
        ShadowSpec(0, 4, 8, 0, dark_glow, 0.05f),
        ShadowSpec(0, 8, 16, 0, dark_glow, 0.02f),
        ShadowSpec(0, 12, 24, 0, dark_glow, 0.01f)};

    // kOverlay: full drop shadow, 32px blur
    shadows_light_[static_cast<size_t>(ElevationLevel::kOverlay)] = {
        ShadowSpec(0, 16, 32, 0, light_shadow, 0.15f)};
    shadows_dark_[static_cast<size_t>(ElevationLevel::kOverlay)] = {
        ShadowSpec(0, 16, 32, 0, dark_glow, 0.1f)};
}

} // namespace markamp::ui
