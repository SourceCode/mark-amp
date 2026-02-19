#include "CanvasTextModel.h"

namespace markamp::canvas
{

void CanvasTextModel::set_text(const std::string& text)
{
    text_ = text;
}
auto CanvasTextModel::text() const -> const std::string&
{
    return text_;
}

void CanvasTextModel::begin_edit()
{
    if (!editing_)
    {
        edit_backup_ = text_;
        editing_ = true;
    }
}

void CanvasTextModel::commit_edit()
{
    editing_ = false;
    edit_backup_.clear();
}

void CanvasTextModel::cancel_edit()
{
    if (editing_)
    {
        text_ = edit_backup_;
        editing_ = false;
        edit_backup_.clear();
    }
}

auto CanvasTextModel::is_editing() const -> bool
{
    return editing_;
}

void CanvasTextModel::set_typography(Typography typography)
{
    typography_ = std::move(typography);
}
auto CanvasTextModel::typography() const -> const Typography&
{
    return typography_;
}

void CanvasTextModel::set_size_mode(TextSizeMode mode)
{
    size_mode_ = mode;
}
auto CanvasTextModel::size_mode() const -> TextSizeMode
{
    return size_mode_;
}

void CanvasTextModel::set_fixed_width(double width)
{
    fixed_width_ = (width > 0) ? width : 1.0;
}
void CanvasTextModel::set_fixed_height(double height)
{
    fixed_height_ = (height > 0) ? height : 1.0;
}
auto CanvasTextModel::fixed_width() const -> double
{
    return fixed_width_;
}
auto CanvasTextModel::fixed_height() const -> double
{
    return fixed_height_;
}

void CanvasTextModel::apply_preset(TextPreset preset)
{
    active_preset_ = preset;
    typography_ = typography_for(preset);
}

auto CanvasTextModel::active_preset() const -> TextPreset
{
    return active_preset_;
}

auto CanvasTextModel::typography_for(TextPreset preset) -> Typography
{
    switch (preset)
    {
        case TextPreset::kHeading:
            return {"Inter", 32.0, 700.0, "#000000", TextAlign::kLeft, 1.2};
        case TextPreset::kBody:
            return {"Inter", 16.0, 400.0, "#333333", TextAlign::kLeft, 1.4};
        case TextPreset::kCallout:
            return {"Inter", 14.0, 600.0, "#0066CC", TextAlign::kCenter, 1.3};
        case TextPreset::kCaption:
            return {"Inter", 12.0, 400.0, "#666666", TextAlign::kLeft, 1.5};
    }
    return {};
}

} // namespace markamp::canvas
