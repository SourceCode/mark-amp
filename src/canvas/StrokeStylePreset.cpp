#include "StrokeStylePreset.h"

namespace markamp::canvas
{

auto StrokeStyle::type_name() const -> std::string
{
    switch (type)
    {
        case StrokeStyleType::kPen:
            return "pen";
        case StrokeStyleType::kMarker:
            return "marker";
        case StrokeStyleType::kHighlighter:
            return "highlighter";
        case StrokeStyleType::kEraser:
            return "eraser";
    }
    return "unknown";
}

StrokeStylePreset::StrokeStylePreset()
{
    default_style_.name = "pen";
    default_style_.type = StrokeStyleType::kPen;
}

void StrokeStylePreset::add_preset(const StrokeStyle& style)
{
    presets_.push_back(style);
}

auto StrokeStylePreset::preset_count() const -> int
{
    return static_cast<int>(presets_.size());
}

auto StrokeStylePreset::find_preset(const std::string& name) const -> const StrokeStyle*
{
    for (const auto& preset : presets_)
    {
        if (preset.name == name)
        {
            return &preset;
        }
    }
    return nullptr;
}

auto StrokeStylePreset::all_presets() const -> const std::vector<StrokeStyle>&
{
    return presets_;
}

auto StrokeStylePreset::set_active(const std::string& name) -> bool
{
    for (size_t i = 0; i < presets_.size(); ++i)
    {
        if (presets_[i].name == name)
        {
            active_index_ = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

auto StrokeStylePreset::active() const -> const StrokeStyle&
{
    if (active_index_ >= 0 && static_cast<size_t>(active_index_) < presets_.size())
    {
        return presets_[static_cast<size_t>(active_index_)];
    }
    return default_style_;
}

auto StrokeStylePreset::active_name() const -> const std::string&
{
    return active().name;
}

void StrokeStylePreset::register_standard_presets()
{
    StrokeStyle pen;
    pen.name = "pen";
    pen.type = StrokeStyleType::kPen;
    pen.width = 2.0;
    pen.opacity = 1.0;
    pen.smoothing = 0.5;
    pen.color_rgba = 0x000000FF;
    add_preset(pen);

    StrokeStyle marker;
    marker.name = "marker";
    marker.type = StrokeStyleType::kMarker;
    marker.width = 8.0;
    marker.opacity = 0.9;
    marker.smoothing = 0.3;
    marker.color_rgba = 0x333333FF;
    add_preset(marker);

    StrokeStyle highlighter;
    highlighter.name = "highlighter";
    highlighter.type = StrokeStyleType::kHighlighter;
    highlighter.width = 16.0;
    highlighter.opacity = 0.4;
    highlighter.smoothing = 0.2;
    highlighter.color_rgba = 0xFFFF00FF;
    add_preset(highlighter);

    StrokeStyle eraser;
    eraser.name = "eraser";
    eraser.type = StrokeStyleType::kEraser;
    eraser.width = 10.0;
    eraser.opacity = 1.0;
    eraser.smoothing = 0.1;
    eraser.color_rgba = 0xFFFFFFFF;
    add_preset(eraser);
}

} // namespace markamp::canvas
