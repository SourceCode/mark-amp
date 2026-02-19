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
};

} // namespace markamp::canvas
