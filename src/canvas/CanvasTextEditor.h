#pragma once

/**
 * @file CanvasTextEditor.h
 * @brief Phase 45 Task 1-3: Inline text editing with typography controls.
 *
 * Edit states (idle/editing/committed/cancelled), typography properties
 * (font, size, weight, alignment, line spacing), auto-size vs fixed-box modes.
 */

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Text editing state.
enum class TextEditState : uint8_t
{
    kIdle,
    kEditing,
    kCommitted,
    kCancelled,
};

/// Text alignment.
enum class TextAlignment : uint8_t
{
    kLeft,
    kCenter,
    kRight,
    kJustify,
};

/// Text sizing mode.
enum class TextSizeMode : uint8_t
{
    kAutoSize, ///< Box grows to fit content
    kFixedBox, ///< Fixed box with text wrapping
};

/// Typography properties.
struct TypographyProps
{
    std::string font_family{"sans-serif"};
    double font_size{14.0};
    int font_weight{400}; ///< 100-900
    TextAlignment alignment{TextAlignment::kLeft};
    double line_spacing{1.4};
    uint32_t color_rgba{0x000000FF};
};

/**
 * @brief Manages canvas text editing lifecycle and typography.
 */
class CanvasTextEditor
{
public:
    CanvasTextEditor() = default;

    // ── Edit state ─────────────────────────────────────────────────

    /// Get current edit state.
    [[nodiscard]] auto state() const -> TextEditState;

    /// Get state as string.
    [[nodiscard]] static auto state_name(TextEditState state) -> std::string;

    /// Begin editing.
    void begin_edit();

    /// Commit edit.
    void commit();

    /// Cancel edit (revert).
    void cancel();

    // ── Content ────────────────────────────────────────────────────

    /// Set text content.
    void set_text(const std::string& text);

    /// Get text content.
    [[nodiscard]] auto text() const -> const std::string&;

    /// Get text length.
    [[nodiscard]] auto text_length() const -> int;

    // ── Typography ─────────────────────────────────────────────────

    /// Set typography properties.
    void set_typography(const TypographyProps& props);

    /// Get typography properties.
    [[nodiscard]] auto typography() const -> const TypographyProps&;

    // ── Sizing ─────────────────────────────────────────────────────

    /// Set sizing mode.
    void set_size_mode(TextSizeMode mode);

    /// Get sizing mode.
    [[nodiscard]] auto size_mode() const -> TextSizeMode;

    /// Set fixed box dimensions (for fixed mode).
    void set_box_size(double width, double height);

    /// Get box width.
    [[nodiscard]] auto box_width() const -> double;

    /// Get box height.
    [[nodiscard]] auto box_height() const -> double;

    /// Estimate content height for current text and width.
    [[nodiscard]] auto estimated_height() const -> double;

private:
    TextEditState state_{TextEditState::kIdle};
    std::string text_;
    std::string pre_edit_text_; ///< For cancel/revert
    TypographyProps typography_;
    TextSizeMode size_mode_{TextSizeMode::kAutoSize};
    double box_width_{200.0};
    double box_height_{50.0};
};

} // namespace markamp::canvas
