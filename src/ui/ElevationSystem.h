#pragma once

#include <wx/colour.h>
#include <wx/graphics.h>

#include <array>
#include <vector>

namespace markamp::ui
{

struct ShadowSpec
{
    int offset_x{0};
    int offset_y{0};
    int blur_radius{0};
    int spread_radius{0};
    wxColour color;
    float opacity{1.0f};

    ShadowSpec() = default;
    ShadowSpec(int ox, int oy, int br, int sr, wxColour col, float a = 1.0f)
        : offset_x(ox)
        , offset_y(oy)
        , blur_radius(br)
        , spread_radius(sr)
        , color(std::move(col))
        , opacity(a)
    {
    }

    bool operator==(const ShadowSpec& other) const
    {
        return offset_x == other.offset_x && offset_y == other.offset_y &&
               blur_radius == other.blur_radius && spread_radius == other.spread_radius &&
               color == other.color && opacity == other.opacity;
    }

    void Render(wxGraphicsContext& gc, const wxRect& bounds) const;
};

enum class ElevationLevel
{
    kNone,
    kLow,
    kMedium,
    kHigh,
    kOverlay,

    // For sizing of internal array
    kCount
};

class ElevationSystem
{
public:
    ElevationSystem();

    void Render(wxGraphicsContext& gc, const wxRect& bounds, ElevationLevel level) const;
    void SetDarkMode(bool dark);

private:
    bool dark_mode_{true};
    std::array<std::vector<ShadowSpec>, static_cast<size_t>(ElevationLevel::kCount)> shadows_light_;
    std::array<std::vector<ShadowSpec>, static_cast<size_t>(ElevationLevel::kCount)> shadows_dark_;

    void InitializeShadows();
};

} // namespace markamp::ui
