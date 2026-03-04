#include "ThemedColorPicker.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace markamp::ui
{

void ColorPickerModel::set_hsv(const HsvColor& hsv)
{
    hsv_ = hsv;
}

void ColorPickerModel::set_rgb(const RgbColor& rgb)
{
    hsv_ = rgb_to_hsv(rgb);
}

void ColorPickerModel::set_hex(const std::string& hex)
{
    std::string clean = hex;
    if (!clean.empty() && clean[0] == '#')
    {
        clean = clean.substr(1);
    }
    if (clean.size() >= 6)
    {
        RgbColor rgb_val;
        rgb_val.red = static_cast<uint8_t>(std::stoul(clean.substr(0, 2), nullptr, 16));
        rgb_val.green = static_cast<uint8_t>(std::stoul(clean.substr(2, 2), nullptr, 16));
        rgb_val.blue = static_cast<uint8_t>(std::stoul(clean.substr(4, 2), nullptr, 16));
        if (clean.size() >= 8)
        {
            rgb_val.alpha = static_cast<uint8_t>(std::stoul(clean.substr(6, 2), nullptr, 16));
        }
        set_rgb(rgb_val);
    }
}

auto ColorPickerModel::rgb() const -> RgbColor
{
    return hsv_to_rgb(hsv_);
}

auto ColorPickerModel::hex() const -> std::string
{
    auto rgb_val = rgb();
    std::ostringstream oss;
    oss << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(rgb_val.red)
        << std::setw(2) << static_cast<int>(rgb_val.green) << std::setw(2)
        << static_cast<int>(rgb_val.blue);
    if (rgb_val.alpha < 255)
    {
        oss << std::setw(2) << static_cast<int>(rgb_val.alpha);
    }
    return oss.str();
}

void ColorPickerModel::set_hue(double hue)
{
    hsv_.hue = std::clamp(hue, 0.0, 360.0);
}
void ColorPickerModel::set_saturation(double saturation)
{
    hsv_.saturation = std::clamp(saturation, 0.0, 1.0);
}
void ColorPickerModel::set_value(double value)
{
    hsv_.value = std::clamp(value, 0.0, 1.0);
}
void ColorPickerModel::set_alpha(double alpha)
{
    hsv_.alpha = std::clamp(alpha, 0.0, 1.0);
}

void ColorPickerModel::save_swatch()
{
    swatches_.push_back(hsv_);
    if (swatches_.size() > 16)
    {
        swatches_.erase(swatches_.begin());
    }
}

auto ColorPickerModel::hsv_to_rgb(const HsvColor& hsv) -> RgbColor
{
    const double chroma = hsv.value * hsv.saturation;
    const double hue_sector = hsv.hue / 60.0;
    const double intermediate = chroma * (1.0 - std::abs(std::fmod(hue_sector, 2.0) - 1.0));
    double red_1 = 0;
    double green_1 = 0;
    double blue_1 = 0;

    if (hue_sector < 1)
    {
        red_1 = chroma;
        green_1 = intermediate;
    }
    else if (hue_sector < 2)
    {
        red_1 = intermediate;
        green_1 = chroma;
    }
    else if (hue_sector < 3)
    {
        green_1 = chroma;
        blue_1 = intermediate;
    }
    else if (hue_sector < 4)
    {
        green_1 = intermediate;
        blue_1 = chroma;
    }
    else if (hue_sector < 5)
    {
        red_1 = intermediate;
        blue_1 = chroma;
    }
    else
    {
        red_1 = chroma;
        blue_1 = intermediate;
    }

    const double match = hsv.value - chroma;
    return {
        .red = static_cast<uint8_t>(std::round((red_1 + match) * 255.0)),
        .green = static_cast<uint8_t>(std::round((green_1 + match) * 255.0)),
        .blue = static_cast<uint8_t>(std::round((blue_1 + match) * 255.0)),
        .alpha = static_cast<uint8_t>(std::round(hsv.alpha * 255.0)),
    };
}

auto ColorPickerModel::rgb_to_hsv(const RgbColor& rgb) -> HsvColor
{
    const double red_n = rgb.red / 255.0;
    const double green_n = rgb.green / 255.0;
    const double blue_n = rgb.blue / 255.0;
    const double max_c = std::max({red_n, green_n, blue_n});
    const double min_c = std::min({red_n, green_n, blue_n});
    const double delta = max_c - min_c;

    double hue = 0;
    if (delta > 0)
    {
        if (max_c == red_n)
        {
            hue = 60.0 * std::fmod((green_n - blue_n) / delta, 6.0);
        }
        else if (max_c == green_n)
        {
            hue = 60.0 * ((blue_n - red_n) / delta + 2.0);
        }
        else
        {
            hue = 60.0 * ((red_n - green_n) / delta + 4.0);
        }
    }
    if (hue < 0)
    {
        hue += 360.0;
    }

    return {
        .hue = hue,
        .saturation = max_c > 0 ? delta / max_c : 0.0,
        .value = max_c,
        .alpha = rgb.alpha / 255.0,
    };
}

} // namespace markamp::ui
