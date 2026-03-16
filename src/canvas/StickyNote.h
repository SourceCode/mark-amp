#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace markamp::canvas
{

/// Color presets for sticky notes.
enum class StickyNoteColor : uint8_t
{
    kYellow,
    kPink,
    kBlue,
    kGreen,
    kOrange,
    kPurple,
    kRed,
    kCyan
};

/// Text alignment for sticky notes.
enum class TextAlign : uint8_t
{
    kLeft,
    kCenter,
    kRight
};

/// Convert a StickyNoteColor enum to an RGBA CanvasColor.
[[nodiscard]] auto sticky_color_to_rgba(StickyNoteColor color) -> CanvasColor;

/// A colored rectangular card with editable multi-line text.
class StickyNote : public CanvasObject
{
public:
    StickyNote();

    // ── Text ───────────────────────────────────────────────────

    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    // ── Note Color ─────────────────────────────────────────────

    [[nodiscard]] auto note_color() const -> StickyNoteColor;
    auto set_note_color(StickyNoteColor color) -> void;

    // ── Font Size ──────────────────────────────────────────────

    [[nodiscard]] auto font_size() const -> double;
    auto set_font_size(double size) -> void;

    // ── Dimensions ─────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto resize(double w, double h) -> void;

    // ── Text Formatting (#1-3) ─────────────────────────────────

    [[nodiscard]] auto is_bold() const -> bool;
    auto set_bold(bool bold) -> void;

    [[nodiscard]] auto is_italic() const -> bool;
    auto set_italic(bool italic) -> void;

    [[nodiscard]] auto text_alignment() const -> TextAlign;
    auto set_text_alignment(TextAlign alignment) -> void;

    // ── Behavior (#4-6) ────────────────────────────────────────

    [[nodiscard]] auto is_pinned() const -> bool;
    auto set_pinned(bool pinned) -> void;

    [[nodiscard]] auto auto_resize() const -> bool;
    auto set_auto_resize(bool enabled) -> void;

    [[nodiscard]] auto character_count() const -> size_t;

    // ── Batch 5 (#25-27) ──────────────────────────────────────────

    /// Count the number of words in the sticky note text.
    [[nodiscard]] auto word_count() const -> size_t;

    /// Return the note text truncated to max_chars with "…".
    [[nodiscard]] auto truncated_text(size_t max_chars) const -> std::string;

    /// Custom font family for this note.
    [[nodiscard]] auto font_family() const -> const std::string&;
    auto set_font_family(const std::string& family) -> void;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

    /// Whether the note has no text.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return text_.empty();
    }

    /// Whether the note has text content.
    [[nodiscard]] auto has_text() const noexcept -> bool
    {
        return !text_.empty();
    }

    /// Whether any text formatting (bold/italic) is applied.
    [[nodiscard]] auto is_formatted() const noexcept -> bool
    {
        return bold_ || italic_;
    }

    /// Whether a custom font family is set.
    [[nodiscard]] auto has_custom_font() const noexcept -> bool
    {
        return font_family_ != "sans-serif";
    }

    // ── Batch 7 (#61-64) ──────────────────────────────────────────

    /// (#61) Count of text lines (newlines + 1).
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

    /// (#62) Whether the note is using the default yellow color.
    [[nodiscard]] auto is_default_color() const noexcept -> bool
    {
        return note_color_ == StickyNoteColor::kYellow;
    }

    /// (#63) Area of the sticky note (width × height).
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width_ * height_;
    }

    /// (#64) Whether both pinned and locked.
    [[nodiscard]] auto is_pinned_and_locked() const noexcept -> bool
    {
        return pinned_ && locked_;
    }

private:
    std::string text_;
    StickyNoteColor note_color_{StickyNoteColor::kYellow};
    double font_size_{14.0};
    double width_{200.0};
    double height_{200.0};
    bool bold_{false};
    bool italic_{false};
    TextAlign text_alignment_{TextAlign::kLeft};
    bool pinned_{false};
    bool auto_resize_{false};
    std::string font_family_{"sans-serif"};

    static constexpr double kMinWidth = 80.0;
    static constexpr double kMinHeight = 80.0;
};

} // namespace markamp::canvas
