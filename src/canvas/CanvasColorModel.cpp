#include "CanvasColorModel.h"

#include <algorithm>
#include <cstdlib>

namespace markamp::canvas
{

void CanvasColorModel::set_palette(std::vector<PaletteSwatch> swatches)
{
    palette_ = std::move(swatches);
}
auto CanvasColorModel::palette() const -> const std::vector<PaletteSwatch>&
{
    return palette_;
}

void CanvasColorModel::push_recent(const std::string& color)
{
    // Remove if already in recent
    recent_.erase(std::remove(recent_.begin(), recent_.end(), color), recent_.end());
    recent_.insert(recent_.begin(), color);
    if (static_cast<int>(recent_.size()) > max_recent_)
    {
        recent_.resize(static_cast<size_t>(max_recent_));
    }
}

auto CanvasColorModel::recent_colors() const -> const std::vector<std::string>&
{
    return recent_;
}
void CanvasColorModel::set_max_recent(int max_count)
{
    max_recent_ = (max_count < 1) ? 1 : max_count;
}

void CanvasColorModel::set_fill_color(CanvasColor color)
{
    fill_color_ = std::move(color);
}
void CanvasColorModel::set_stroke_color(CanvasColor color)
{
    stroke_color_ = std::move(color);
}
auto CanvasColorModel::fill_color() const -> const CanvasColor&
{
    return fill_color_;
}
auto CanvasColorModel::stroke_color() const -> const CanvasColor&
{
    return stroke_color_;
}

void CanvasColorModel::copy_style(const std::string& fill, const std::string& stroke)
{
    copied_fill_ = fill;
    copied_stroke_ = stroke;
    has_copied_ = true;
}

auto CanvasColorModel::has_copied_style() const -> bool
{
    return has_copied_;
}
auto CanvasColorModel::copied_fill() const -> const std::string&
{
    return copied_fill_;
}
auto CanvasColorModel::copied_stroke() const -> const std::string&
{
    return copied_stroke_;
}

static auto hex_to_luminance(const std::string& hex) -> double
{
    if (hex.size() < 7 || hex[0] != '#')
    {
        return 0.5;
    }
    const int red = static_cast<int>(std::strtol(hex.substr(1, 2).c_str(), nullptr, 16));
    const int green = static_cast<int>(std::strtol(hex.substr(3, 2).c_str(), nullptr, 16));
    const int blue = static_cast<int>(std::strtol(hex.substr(5, 2).c_str(), nullptr, 16));
    return (0.299 * red + 0.587 * green + 0.114 * blue) / 255.0;
}

auto CanvasColorModel::has_contrast_warning(const std::string& text_color,
                                            const std::string& bg_color) -> bool
{
    const double text_lum = hex_to_luminance(text_color);
    const double bg_lum = hex_to_luminance(bg_color);
    const double contrast = std::abs(text_lum - bg_lum);
    return contrast < 0.3; // Low contrast threshold
}

} // namespace markamp::canvas
