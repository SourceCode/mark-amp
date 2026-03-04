#pragma once

/**
 * @file TextStylePreset.h
 * @brief Phase 45 Task 4: Text style presets (heading/body/callout/code).
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Text preset type.
enum class TextPresetType : uint8_t
{
    kHeading,
    kBody,
    kCallout,
    kCode,
};

/// A text style preset.
struct TextPreset
{
    std::string name;
    TextPresetType type{TextPresetType::kBody};
    std::string font_family{"sans-serif"};
    double font_size{14.0};
    int font_weight{400};
    uint32_t color_rgba{0x000000FF};

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/**
 * @brief Manages text style presets with quick switching.
 */
class TextStylePreset
{
public:
    TextStylePreset() = default;

    /// Add a preset.
    void add_preset(const TextPreset& preset);

    /// Get preset count.
    [[nodiscard]] auto preset_count() const -> int;

    /// Find a preset by name.
    [[nodiscard]] auto find_preset(const std::string& name) const -> const TextPreset*;

    /// Set active preset. Returns true if found.
    auto set_active(const std::string& name) -> bool;

    /// Get active preset.
    [[nodiscard]] auto active() const -> const TextPreset&;

    /// Register standard presets (heading, body, callout, code).
    void register_standard_presets();

private:
    std::vector<TextPreset> presets_;
    int active_index_{0};
    TextPreset default_preset_;
};

} // namespace markamp::canvas
