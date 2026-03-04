#include "ColorPaletteController.h"

#include <algorithm>

namespace markamp::canvas
{

void ColorPaletteController::add_swatch(const ColorSwatch& swatch)
{
    palette_.push_back(swatch);
}

auto ColorPaletteController::palette_size() const -> int
{
    return static_cast<int>(palette_.size());
}

auto ColorPaletteController::palette() const -> const std::vector<ColorSwatch>&
{
    return palette_;
}

void ColorPaletteController::register_default_palette()
{
    add_swatch({"black", 0x000000FF});
    add_swatch({"white", 0xFFFFFFFF});
    add_swatch({"red", 0xFF0000FF});
    add_swatch({"green", 0x00CC00FF});
    add_swatch({"blue", 0x0066FFFF});
    add_swatch({"yellow", 0xFFCC00FF});
    add_swatch({"orange", 0xFF6600FF});
    add_swatch({"purple", 0x9900CCFF});
}

void ColorPaletteController::record_recent(uint32_t rgba)
{
    // Remove duplicates
    recent_.erase(std::remove(recent_.begin(), recent_.end(), rgba), recent_.end());

    // Add to front
    recent_.insert(recent_.begin(), rgba);

    // Trim to max
    if (static_cast<int>(recent_.size()) > max_recent_)
    {
        recent_.resize(static_cast<size_t>(max_recent_));
    }
}

auto ColorPaletteController::recent_colors() const -> const std::vector<uint32_t>&
{
    return recent_;
}

auto ColorPaletteController::recent_count() const -> int
{
    return static_cast<int>(recent_.size());
}

void ColorPaletteController::set_max_recent(int max_count)
{
    max_recent_ = max_count;
}

void ColorPaletteController::copy_style(const CopiedStyle& style)
{
    copied_style_ = style;
    has_copied_ = true;
}

auto ColorPaletteController::has_copied_style() const -> bool
{
    return has_copied_;
}

auto ColorPaletteController::copied_style() const -> const CopiedStyle&
{
    return copied_style_;
}

void ColorPaletteController::clear_copied_style()
{
    has_copied_ = false;
}

} // namespace markamp::canvas
