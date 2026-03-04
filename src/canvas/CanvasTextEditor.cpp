#include "CanvasTextEditor.h"

namespace markamp::canvas
{

auto CanvasTextEditor::state() const -> TextEditState
{
    return state_;
}

auto CanvasTextEditor::state_name(TextEditState state) -> std::string
{
    switch (state)
    {
        case TextEditState::kIdle:
            return "idle";
        case TextEditState::kEditing:
            return "editing";
        case TextEditState::kCommitted:
            return "committed";
        case TextEditState::kCancelled:
            return "cancelled";
    }
    return "unknown";
}

void CanvasTextEditor::begin_edit()
{
    pre_edit_text_ = text_;
    state_ = TextEditState::kEditing;
}

void CanvasTextEditor::commit()
{
    state_ = TextEditState::kCommitted;
}

void CanvasTextEditor::cancel()
{
    text_ = pre_edit_text_;
    state_ = TextEditState::kCancelled;
}

void CanvasTextEditor::set_text(const std::string& text)
{
    text_ = text;
}

auto CanvasTextEditor::text() const -> const std::string&
{
    return text_;
}

auto CanvasTextEditor::text_length() const -> int
{
    return static_cast<int>(text_.size());
}

void CanvasTextEditor::set_typography(const TypographyProps& props)
{
    typography_ = props;
}

auto CanvasTextEditor::typography() const -> const TypographyProps&
{
    return typography_;
}

void CanvasTextEditor::set_size_mode(TextSizeMode mode)
{
    size_mode_ = mode;
}

auto CanvasTextEditor::size_mode() const -> TextSizeMode
{
    return size_mode_;
}

void CanvasTextEditor::set_box_size(double width, double height)
{
    box_width_ = width;
    box_height_ = height;
}

auto CanvasTextEditor::box_width() const -> double
{
    return box_width_;
}

auto CanvasTextEditor::box_height() const -> double
{
    return box_height_;
}

auto CanvasTextEditor::estimated_height() const -> double
{
    if (text_.empty())
    {
        return typography_.font_size * typography_.line_spacing;
    }

    // Approximate: chars per line based on font size and box width
    double chars_per_line = box_width_ / (typography_.font_size * 0.6);
    if (chars_per_line < 1.0)
    {
        chars_per_line = 1.0;
    }

    double line_count = static_cast<double>(text_.size()) / chars_per_line;
    if (line_count < 1.0)
    {
        line_count = 1.0;
    }

    return line_count * typography_.font_size * typography_.line_spacing;
}

} // namespace markamp::canvas
