#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Color type (semantic or literal).
enum class ColorType : uint8_t
{
    kLiteral,  ///< Direct hex color
    kSemantic, ///< References a theme token
};

/// A canvas color entry.
struct CanvasColor
{
    std::string value; ///< Hex value or token name
    ColorType type{ColorType::kLiteral};
};

/// Palette swatch.
struct PaletteSwatch
{
    std::string color;
    std::string label;
};

/// Testable model for Canvas Color System (Phase 46).
///
/// Encapsulates:
/// - Palette with quick swatches
/// - Recent colors stack (MRU)
/// - Style copy source/target
/// - Text/fill contrast warning
class CanvasColorModel
{
public:
    // ── Palette ─────────────────────────────────────────────────────

    void set_palette(std::vector<PaletteSwatch> swatches);
    [[nodiscard]] auto palette() const -> const std::vector<PaletteSwatch>&;

    // ── Recent colors ───────────────────────────────────────────────

    void push_recent(const std::string& color);
    [[nodiscard]] auto recent_colors() const -> const std::vector<std::string>&;
    void set_max_recent(int max_count);

    // ── Active color ────────────────────────────────────────────────

    void set_fill_color(CanvasColor color);
    void set_stroke_color(CanvasColor color);
    [[nodiscard]] auto fill_color() const -> const CanvasColor&;
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;

    // ── Style copy ──────────────────────────────────────────────────

    void copy_style(const std::string& fill, const std::string& stroke);
    [[nodiscard]] auto has_copied_style() const -> bool;
    [[nodiscard]] auto copied_fill() const -> const std::string&;
    [[nodiscard]] auto copied_stroke() const -> const std::string&;

    // ── Contrast ────────────────────────────────────────────────────

    /// Simple luminance-based contrast check.
    [[nodiscard]] static auto has_contrast_warning(const std::string& text_color,
                                                   const std::string& bg_color) -> bool;

private:
    std::vector<PaletteSwatch> palette_;
    std::vector<std::string> recent_;
    int max_recent_{10};
    CanvasColor fill_color_{"#FFFFFF", ColorType::kLiteral};
    CanvasColor stroke_color_{"#000000", ColorType::kLiteral};
    std::string copied_fill_;
    std::string copied_stroke_;
    bool has_copied_{false};
};

} // namespace markamp::canvas
