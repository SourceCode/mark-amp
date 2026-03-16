#include "CanvasTextEditor.h"

#include <algorithm>

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

void CanvasTextEditor::set_caret_position(int position)
{
    caret_position_ = std::clamp(position, 0, static_cast<int>(text_.size()));
    selection_start_ = caret_position_;
    selection_end_ = caret_position_;
}

auto CanvasTextEditor::caret_position() const -> int
{
    return caret_position_;
}

void CanvasTextEditor::select_range(int start, int end)
{
    const int max_pos = static_cast<int>(text_.size());
    selection_start_ = std::clamp(start, 0, max_pos);
    selection_end_ = std::clamp(end, 0, max_pos);
    caret_position_ = selection_end_;
}

auto CanvasTextEditor::selection_start() const -> int
{
    return selection_start_;
}

auto CanvasTextEditor::selection_end() const -> int
{
    return selection_end_;
}

auto CanvasTextEditor::has_selection() const -> bool
{
    return selection_start_ != selection_end_;
}

auto CanvasTextEditor::selected_text() const -> std::string
{
    if (!has_selection())
    {
        return {};
    }
    const int from = std::min(selection_start_, selection_end_);
    const int to = std::max(selection_start_, selection_end_);
    return text_.substr(static_cast<size_t>(from),
                        static_cast<size_t>(to - from));
}

auto CanvasTextEditor::caret_position_from_click(double click_x) const -> int
{
    // Approximate character position from click X coordinate.
    // Each character is roughly font_size * 0.6 wide.
    const double char_width = typography_.font_size * 0.6;
    if (char_width < 1.0)
    {
        return 0;
    }
    const int char_pos = static_cast<int>(click_x / char_width);
    return std::clamp(char_pos, 0, static_cast<int>(text_.size()));
}

} // namespace markamp::canvas
