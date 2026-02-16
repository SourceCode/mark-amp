#include "StickyNote.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

auto sticky_color_to_rgba(StickyNoteColor color) -> CanvasColor
{
    switch (color)
    {
        case StickyNoteColor::kYellow:
            return CanvasColor{255, 249, 196, 255}; // #FFF9C4
        case StickyNoteColor::kPink:
            return CanvasColor{248, 187, 208, 255}; // #F8BBD0
        case StickyNoteColor::kBlue:
            return CanvasColor{187, 222, 251, 255}; // #BBDEFB
        case StickyNoteColor::kGreen:
            return CanvasColor{200, 230, 201, 255}; // #C8E6C9
        case StickyNoteColor::kOrange:
            return CanvasColor{255, 224, 178, 255}; // #FFE0B2
        case StickyNoteColor::kPurple:
            return CanvasColor{225, 190, 231, 255}; // #E1BEE7
        case StickyNoteColor::kRed:
            return CanvasColor{255, 205, 210, 255}; // #FFCDD2
        case StickyNoteColor::kCyan:
            return CanvasColor{178, 235, 242, 255}; // #B2EBF2
    }
    return CanvasColor{255, 249, 196, 255}; // Default yellow.
}

StickyNote::StickyNote()
    : CanvasObject(CanvasObjectType::StickyNote)
{
}

auto StickyNote::text() const -> const std::string&
{
    return text_;
}
auto StickyNote::set_text(const std::string& text) -> void
{
    text_ = text;
    mark_dirty();
}

auto StickyNote::note_color() const -> StickyNoteColor
{
    return note_color_;
}
auto StickyNote::set_note_color(StickyNoteColor color) -> void
{
    note_color_ = color;
    mark_dirty();
}

auto StickyNote::font_size() const -> double
{
    return font_size_;
}
auto StickyNote::set_font_size(double size) -> void
{
    font_size_ = std::max(8.0, size);
    mark_dirty();
}

auto StickyNote::width() const -> double
{
    return width_;
}
auto StickyNote::height() const -> double
{
    return height_;
}

auto StickyNote::resize(double w, double h) -> void
{
    width_ = std::max(kMinWidth, w);
    height_ = std::max(kMinHeight, h);
    mark_dirty();
}

// ── Text Formatting (#1-3) ─────────────────────────────────

auto StickyNote::is_bold() const -> bool
{
    return bold_;
}
auto StickyNote::set_bold(bool bold) -> void
{
    bold_ = bold;
    mark_dirty();
}
auto StickyNote::is_italic() const -> bool
{
    return italic_;
}
auto StickyNote::set_italic(bool italic) -> void
{
    italic_ = italic;
    mark_dirty();
}
auto StickyNote::text_alignment() const -> TextAlign
{
    return text_alignment_;
}
auto StickyNote::set_text_alignment(TextAlign alignment) -> void
{
    text_alignment_ = alignment;
    mark_dirty();
}

// ── Behavior (#4-6) ────────────────────────────────────────

auto StickyNote::is_pinned() const -> bool
{
    return pinned_;
}
auto StickyNote::set_pinned(bool pinned) -> void
{
    pinned_ = pinned;
    mark_dirty();
}
auto StickyNote::auto_resize() const -> bool
{
    return auto_resize_;
}
auto StickyNote::set_auto_resize(bool enabled) -> void
{
    auto_resize_ = enabled;
    mark_dirty();
}
auto StickyNote::character_count() const -> size_t
{
    return text_.size();
}

// --- Batch 5 (#25-27) ---

auto StickyNote::word_count() const -> size_t
{
    if (text_.empty())
    {
        return 0;
    }
    size_t count = 0;
    bool in_word = false;
    for (const char ch : text_)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true;
            ++count;
        }
    }
    return count;
}

auto StickyNote::truncated_text(size_t max_chars) const -> std::string
{
    if (text_.size() <= max_chars)
    {
        return text_;
    }
    if (max_chars < 3)
    {
        return "...";
    }
    return text_.substr(0, max_chars - 3) + "...";
}

auto StickyNote::font_family() const -> const std::string&
{
    return font_family_;
}

auto StickyNote::set_font_family(const std::string& family) -> void
{
    font_family_ = family;
    mark_dirty();
}

auto StickyNote::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto StickyNote::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<StickyNote>();
    copy->text_ = text_;
    copy->note_color_ = note_color_;
    copy->font_size_ = font_size_;
    copy->width_ = width_;
    copy->height_ = height_;
    copy->bold_ = bold_;
    copy->italic_ = italic_;
    copy->text_alignment_ = text_alignment_;
    copy->pinned_ = pinned_;
    copy->auto_resize_ = auto_resize_;
    copy->font_family_ = font_family_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto StickyNote::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"StickyNote\""
        << ",\"text\":\"" << text_ << "\""
        << ",\"color\":" << static_cast<int>(note_color_) << ",\"font_size\":" << font_size_
        << ",\"width\":" << width_ << ",\"height\":" << height_ << "}";
    return oss.str();
}

auto StickyNote::from_json(const std::string& /*json*/) -> void
{
    // Stub: real JSON parsing would populate fields.
}

} // namespace markamp::canvas
