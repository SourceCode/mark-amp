#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

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

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    std::string text_;
    StickyNoteColor note_color_{StickyNoteColor::kYellow};
    double font_size_{14.0};
    double width_{200.0};
    double height_{200.0};

    static constexpr double kMinWidth = 80.0;
    static constexpr double kMinHeight = 80.0;
};

} // namespace markamp::canvas
