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

    static auto hover_variant(const wxColour& base, bool is_dark_mode) -> wxColour;
    static auto pressed_variant(const wxColour& base, bool is_dark_mode) -> wxColour;
    static auto disabled_variant(const wxColour& base) -> wxColour;
    static auto focus_ring(const wxColour& accent) -> wxColour;

    [[nodiscard]] static auto generate_extended_palette(const core::Theme& base_theme)
        -> ExtendedPalette;
};

} // namespace markamp::ui
