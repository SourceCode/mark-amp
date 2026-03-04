#pragma once

/**
 * @file ColorPaletteController.h
 * @brief Phase 46 Task 2-3: Color palette, recent colors, and style copy/paste.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A color swatch in the palette.
struct ColorSwatch
{
    std::string name;
    uint32_t rgba{0x000000FF};
};

/// Copied object style for paste operations.
struct CopiedStyle
{
    uint32_t fill_color{0xFFFFFFFF};
    uint32_t stroke_color{0x000000FF};
    uint32_t text_color{0x000000FF};
    bool has_fill{true};
    bool has_stroke{true};
    bool has_text{false};
};

/**
 * @brief Manages color palette, recent colors, and style transfer.
 */
class ColorPaletteController
{
public:
    ColorPaletteController() = default;

    // ── Palette ────────────────────────────────────────────────────

    /// Add a swatch to the palette.
    void add_swatch(const ColorSwatch& swatch);

    /// Get palette size.
    [[nodiscard]] auto palette_size() const -> int;

    /// Get all swatches.
    [[nodiscard]] auto palette() const -> const std::vector<ColorSwatch>&;

    /// Register default palette.
    void register_default_palette();

    // ── Recent colors ──────────────────────────────────────────────

    /// Record a recently used color.
    void record_recent(uint32_t rgba);

    /// Get recent colors.
    [[nodiscard]] auto recent_colors() const -> const std::vector<uint32_t>&;

    /// Get recent color count.
    [[nodiscard]] auto recent_count() const -> int;

    /// Set max recent colors tracked.
    void set_max_recent(int max_count);

    // ── Style copy/paste ───────────────────────────────────────────

    /// Copy style from an object.
    void copy_style(const CopiedStyle& style);

    /// Check if a style is copied.
    [[nodiscard]] auto has_copied_style() const -> bool;

    /// Get the copied style.
    [[nodiscard]] auto copied_style() const -> const CopiedStyle&;

    /// Clear copied style.
    void clear_copied_style();

private:
    std::vector<ColorSwatch> palette_;
    std::vector<uint32_t> recent_;
    int max_recent_{10};
    CopiedStyle copied_style_;
    bool has_copied_{false};
};

} // namespace markamp::canvas
