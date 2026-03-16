#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Text sizing mode.
enum class TextSizeMode : uint8_t
{
    kAutoSize, ///< Box grows to fit content
    kFixedBox, ///< Fixed dimensions, text wraps
};

/// Text alignment.
enum class TextAlign : uint8_t
{
    kLeft,
    kCenter,
    kRight,
};

/// Text style preset type.
enum class TextPreset : uint8_t
{
    kHeading,
    kBody,
    kCallout,
    kCaption,
};

/// Typography properties.
struct Typography
{
    std::string font_family{"Inter"};
    double font_size{16.0};
    double font_weight{400.0};
    std::string color{"#000000"};
    TextAlign align{TextAlign::kLeft};
    double line_spacing{1.4};

    // ── Round 4 Batch 3-4 (#29-33) ──────────────────────────────

    /// (#29) Whether the weight qualifies as bold (>= 700).
    [[nodiscard]] auto is_bold() const noexcept -> bool
    {
        return font_weight >= 700.0;
    }

    /// (#30) Whether text is center-aligned.
    [[nodiscard]] auto is_centered() const noexcept -> bool
    {
        return align == TextAlign::kCenter;
    }

    /// (#31) Whether text is left-aligned.
    [[nodiscard]] auto is_left_aligned() const noexcept -> bool
    {
        return align == TextAlign::kLeft;
    }

    /// (#32) Whether text is right-aligned.
    [[nodiscard]] auto is_right_aligned() const noexcept -> bool
    {
        return align == TextAlign::kRight;
    }

    /// (#33) Whether a font family is set.
    [[nodiscard]] auto has_font() const noexcept -> bool
    {
        return !font_family.empty();
    }
};

/// Testable model for Canvas Text Editing & Typography (Phase 45).
///
/// Encapsulates:
/// - Text content with editing lifecycle (enter/commit/cancel)
/// - Typography controls (font, size, weight, color, align, spacing)
/// - Auto-size vs fixed-box modes
/// - Text style presets (heading/body/callout/caption)
class CanvasTextModel
{
public:
    // ── Content ─────────────────────────────────────────────────────

    void set_text(const std::string& text);
    [[nodiscard]] auto text() const -> const std::string&;

    void begin_edit();
    void commit_edit();
    void cancel_edit();
    [[nodiscard]] auto is_editing() const -> bool;

    // ── Typography ──────────────────────────────────────────────────

    void set_typography(Typography typography);
    [[nodiscard]] auto typography() const -> const Typography&;

    // ── Size mode ───────────────────────────────────────────────────

    void set_size_mode(TextSizeMode mode);
    [[nodiscard]] auto size_mode() const -> TextSizeMode;

    void set_fixed_width(double width);
    void set_fixed_height(double height);
    [[nodiscard]] auto fixed_width() const -> double;
    [[nodiscard]] auto fixed_height() const -> double;

    // ── Presets ──────────────────────────────────────────────────────

    void apply_preset(TextPreset preset);
    [[nodiscard]] auto active_preset() const -> TextPreset;

    /// Get the typography for a preset.
    [[nodiscard]] static auto typography_for(TextPreset preset) -> Typography;

private:
    std::string text_;
    std::string edit_backup_;
    bool editing_{false};
    Typography typography_;
    TextSizeMode size_mode_{TextSizeMode::kAutoSize};
    double fixed_width_{200.0};
    double fixed_height_{100.0};
    TextPreset active_preset_{TextPreset::kBody};

    // ── Round 4 Batch 4 (#34-37) ────────────────────────────────

    /// (#34) Whether auto-size mode is active.
    [[nodiscard]] auto is_auto_size() const noexcept -> bool
    {
        return size_mode_ == TextSizeMode::kAutoSize;
    }

    /// (#35) Whether fixed-box mode is active.
    [[nodiscard]] auto is_fixed_box() const noexcept -> bool
    {
        return size_mode_ == TextSizeMode::kFixedBox;
    }

    /// (#36) Whether text content is present.
    [[nodiscard]] auto has_text() const noexcept -> bool
    {
        return !text_.empty();
    }

    /// (#37) Whether the active preset is heading.
    [[nodiscard]] auto is_heading() const noexcept -> bool
    {
        return active_preset_ == TextPreset::kHeading;
    }
};

} // namespace markamp::canvas
