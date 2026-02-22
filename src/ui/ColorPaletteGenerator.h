#pragma once
#include "../core/ThemeEngine.h"

#include <wx/colour.h>

#include <unordered_map>

namespace markamp::ui
{

using ExtendedPalette = std::unordered_map<core::ThemeColorToken, wxColour>;

class ColorPaletteGenerator
{
public:
    ColorPaletteGenerator() = default;

    auto hover_variant(const wxColour& base, bool is_dark_mode) const -> wxColour;
    auto pressed_variant(const wxColour& base, bool is_dark_mode) const -> wxColour;
    auto disabled_variant(const wxColour& base) const -> wxColour;
    auto focus_ring(const wxColour& accent) const -> wxColour;

    auto generate_extended_palette(const core::Theme& base_theme) const -> ExtendedPalette;
};

} // namespace markamp::ui
