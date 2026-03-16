#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>

namespace markamp::canvas
{

/// Text alignment within a text box.
enum class TextAlignment : uint8_t
{
    kLeft,
    kCenter,
    kRight
};

/// Style properties for text rendering.
struct TextStyle
{
    std::string font_family{"sans-serif"};
    double font_size{14.0};
    bool bold{false};
    bool italic{false};
    bool underline{false};
    CanvasColor text_color{51, 51, 51, 255};
    TextAlignment alignment{TextAlignment::kLeft};

    /// Whether the text style uses bold.
    [[nodiscard]] auto is_bold() const noexcept -> bool
    {
        return bold;
    }

    /// Whether any formatting (bold/italic/underline) is applied.
    [[nodiscard]] auto is_styled() const noexcept -> bool
    {
        return bold || italic || underline;
    }
};

/// A free-floating text box with configurable font, size, weight, alignment, border, and fill.
class TextBox : public CanvasObject
{
public:
    TextBox();

    // ── Text ───────────────────────────────────────────────────

    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    // ── Style ──────────────────────────────────────────────────

    [[nodiscard]] auto style() const -> const TextStyle&;
    auto set_style(const TextStyle& style) -> void;

    // ── Dimensions ─────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto resize(double w, double h) -> void;

    /// Calculate height from text content and style (estimated).
    [[nodiscard]] auto auto_height() const -> double;

    // ── Border & Fill ──────────────────────────────────────────

    [[nodiscard]] auto has_border() const -> bool;
    auto set_has_border(bool enabled) -> void;
    [[nodiscard]] auto border_color() const -> CanvasColor;
    auto set_border_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto has_fill() const -> bool;
    auto set_has_fill(bool enabled) -> void;
    [[nodiscard]] auto fill_color() const -> CanvasColor;
    auto set_fill_color(const CanvasColor& color) -> void;

    // ── Batch 5 (#28-30) ──────────────────────────────────────────

    /// Count lines (newlines + 1) in the text box content.
    [[nodiscard]] auto line_count() const -> size_t;

    /// Custom line spacing multiplier (default 1.4).
    [[nodiscard]] auto line_spacing() const -> double;
    auto set_line_spacing(double spacing) -> void;

    /// Custom inner padding (default 10.0).
    [[nodiscard]] auto padding() const -> double;
    auto set_padding(double pad) -> void;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

    /// Whether the text box is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return text_.empty();
    }

    /// Whether the text box has content.
    [[nodiscard]] auto has_text() const noexcept -> bool
    {
        return !text_.empty();
    }

    // ── Batch 7 (#65-68) ──────────────────────────────────────────

    /// (#65) Number of characters in the text.
    [[nodiscard]] auto character_count() const noexcept -> size_t
    {
        return text_.size();
    }

    /// (#66) Number of text lines (newlines + 1).
    [[nodiscard]] auto text_line_count() const noexcept -> size_t
    {
        if (text_.empty()) { return 0; }
        size_t count = 1;
        for (const char chr : text_)
        {
            if (chr == '\n') { ++count; }
        }
        return count;
    }

    /// (#67) Area of the text box (width × height).
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width_ * height_;
    }

    /// (#68) Whether the text box has a fill.
    [[nodiscard]] auto is_filled() const noexcept -> bool
    {
        return has_fill_;
    }

private:
    std::string text_;
    TextStyle style_;
    double width_{200.0};
    double height_{100.0};
    bool has_border_{false};
    CanvasColor border_color_{0, 0, 0, 255};
    bool has_fill_{false};
    CanvasColor fill_color_{255, 255, 255, 255};
    double line_spacing_{1.4};
    double padding_{10.0};

    static constexpr double kMinWidth = 40.0;
    static constexpr double kMinHeight = 20.0;
    static constexpr double kLineHeightMultiplier = 1.4;
    static constexpr double kPadding = 10.0;
};

} // namespace markamp::canvas
