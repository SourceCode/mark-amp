#pragma once

/**
 * @file StrokeStylePreset.h
 * @brief Phase 42 Task 3-4: Stroke style presets (pen/marker/highlighter/eraser).
 *
 * Style types with parameters for width, opacity, smoothing, and color.
 * Quick preset switching with persistence.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Stroke style type.
enum class StrokeStyleType : uint8_t
{
    kPen,
    kMarker,
    kHighlighter,
    kEraser,
};

/// Stroke style parameters.
struct StrokeStyle
{
    std::string name;
    StrokeStyleType type{StrokeStyleType::kPen};
    double width{2.0};
    double opacity{1.0};
    double smoothing{0.5};
    uint32_t color_rgba{0x000000FF}; ///< RGBA packed

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/**
 * @brief Manages stroke style presets with quick switching.
 */
class StrokeStylePreset
{
public:
    StrokeStylePreset();

    // ── Preset management ──────────────────────────────────────────

    /// Register a style preset.
    void add_preset(const StrokeStyle& style);

    /// Get preset count.
    [[nodiscard]] auto preset_count() const -> int;

    /// Get a preset by name.
    [[nodiscard]] auto find_preset(const std::string& name) const -> const StrokeStyle*;

    /// Get all presets.
    [[nodiscard]] auto all_presets() const -> const std::vector<StrokeStyle>&;

    // ── Active preset ──────────────────────────────────────────────

    /// Set active preset by name. Returns true if found.
    auto set_active(const std::string& name) -> bool;

    /// Get active preset.
    [[nodiscard]] auto active() const -> const StrokeStyle&;

    /// Get active preset name.
    [[nodiscard]] auto active_name() const -> const std::string&;

    // ── Standard presets ───────────────────────────────────────────

    /// Register standard presets (pen, marker, highlighter, eraser).
    void register_standard_presets();

private:
    std::vector<StrokeStyle> presets_;
    int active_index_{0};
    StrokeStyle default_style_;
};

} // namespace markamp::canvas
