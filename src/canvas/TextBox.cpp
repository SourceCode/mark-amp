#include "TextBox.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

TextBox::TextBox()
    : CanvasObject(CanvasObjectType::TextBox)
{
}

// ── Text ───────────────────────────────────────────────────────

auto TextBox::text() const -> const std::string&
{
    return text_;
}
auto TextBox::set_text(const std::string& text) -> void
{
    text_ = text;
    mark_dirty();
}

// ── Style ──────────────────────────────────────────────────────

auto TextBox::style() const -> const TextStyle&
{
    return style_;
}
auto TextBox::set_style(const TextStyle& style) -> void
{
    style_ = style;
    mark_dirty();
}

// ── Dimensions ─────────────────────────────────────────────────

auto TextBox::width() const -> double
{
    return width_;
}
auto TextBox::height() const -> double
{
    return height_;
}

auto TextBox::resize(double w, double h) -> void
{
    width_ = std::max(kMinWidth, w);
    height_ = std::max(kMinHeight, h);
    mark_dirty();
}

auto TextBox::auto_height() const -> double
{
    if (text_.empty())
    {
        return style_.font_size * kLineHeightMultiplier + kPadding * 2.0;
    }

    // Count lines.
    size_t line_count = 1;
    for (const char ch : text_)
    {
        if (ch == '\n')
        {
            ++line_count;
        }
    }

    return static_cast<double>(line_count) * style_.font_size * kLineHeightMultiplier +
           kPadding * 2.0;
}

// ── Border & Fill ──────────────────────────────────────────────

auto TextBox::has_border() const -> bool
{
    return has_border_;
}
auto TextBox::set_has_border(bool enabled) -> void
{
    has_border_ = enabled;
    mark_dirty();
}
auto TextBox::border_color() const -> CanvasColor
{
    return border_color_;
}
auto TextBox::set_border_color(const CanvasColor& color) -> void
{
    border_color_ = color;
    mark_dirty();
}

auto TextBox::has_fill() const -> bool
{
    return has_fill_;
}
auto TextBox::set_has_fill(bool enabled) -> void
{
    has_fill_ = enabled;
    mark_dirty();
}
auto TextBox::fill_color() const -> CanvasColor
{
    return fill_color_;
}
auto TextBox::set_fill_color(const CanvasColor& color) -> void
{
    fill_color_ = color;
    mark_dirty();
}

// --- Batch 5 (#28-30) ---

auto TextBox::line_count() const -> size_t
{
    if (text_.empty())
    {
        return 0;
    }
    size_t count = 1;
    for (const char ch : text_)
    {
        if (ch == '\n')
        {
            ++count;
        }
    }
    return count;
}

auto TextBox::line_spacing() const -> double
{
    return line_spacing_;
}

auto TextBox::set_line_spacing(double spacing) -> void
{
    line_spacing_ = std::max(0.5, spacing);
    mark_dirty();
}

auto TextBox::padding() const -> double
{
    return padding_;
}

auto TextBox::set_padding(double pad) -> void
{
    padding_ = std::max(0.0, pad);
    mark_dirty();
}

// ── CanvasObject overrides ─────────────────────────────────────

auto TextBox::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto TextBox::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<TextBox>();
    copy->text_ = text_;
    copy->style_ = style_;
    copy->width_ = width_;
    copy->height_ = height_;
    copy->has_border_ = has_border_;
    copy->border_color_ = border_color_;
    copy->has_fill_ = has_fill_;
    copy->fill_color_ = fill_color_;
    copy->line_spacing_ = line_spacing_;
    copy->padding_ = padding_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto TextBox::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"TextBox\""
        << ",\"text\":\"" << text_ << "\""
        << ",\"font_size\":" << style_.font_size << ",\"bold\":" << (style_.bold ? "true" : "false")
        << ",\"italic\":" << (style_.italic ? "true" : "false") << ",\"width\":" << width_
        << ",\"height\":" << height_ << "}";
    return oss.str();
}

auto TextBox::from_json(const std::string& /*json*/) -> void
{
    // Stub.
}

} // namespace markamp::canvas
