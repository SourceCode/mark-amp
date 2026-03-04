#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Phase 41: HSV color model for the color picker.
struct HsvColor
{
    double hue{0.0};        ///< 0–360
    double saturation{1.0}; ///< 0–1
    double value{1.0};      ///< 0–1
    double alpha{1.0};      ///< 0–1
};

struct RgbColor
{
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
    uint8_t alpha{255};
};

/// Phase 41: Pure model for the color picker.
class ColorPickerModel
{
public:
    void set_hsv(const HsvColor& hsv);
    [[nodiscard]] auto hsv() const -> const HsvColor&
    {
        return hsv_;
    }

    void set_rgb(const RgbColor& rgb);
    [[nodiscard]] auto rgb() const -> RgbColor;

    void set_hex(const std::string& hex);
    [[nodiscard]] auto hex() const -> std::string;

    void set_hue(double hue);
    void set_saturation(double saturation);
    void set_value(double value);
    void set_alpha(double alpha);

    /// Add to recent swatches.
    void save_swatch();
    [[nodiscard]] auto recent_swatches() const -> const std::vector<HsvColor>&
    {
        return swatches_;
    }

    /// Static conversion utilities.
    static auto hsv_to_rgb(const HsvColor& hsv) -> RgbColor;
    static auto rgb_to_hsv(const RgbColor& rgb) -> HsvColor;

private:
    HsvColor hsv_;
    std::vector<HsvColor> swatches_;
};

} // namespace markamp::ui
