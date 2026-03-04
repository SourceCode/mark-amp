#include "TextStylePreset.h"

namespace markamp::canvas
{

auto TextPreset::type_name() const -> std::string
{
    switch (type)
    {
        case TextPresetType::kHeading:
            return "heading";
        case TextPresetType::kBody:
            return "body";
        case TextPresetType::kCallout:
            return "callout";
        case TextPresetType::kCode:
            return "code";
    }
    return "unknown";
}

void TextStylePreset::add_preset(const TextPreset& preset)
{
    presets_.push_back(preset);
}

auto TextStylePreset::preset_count() const -> int
{
    return static_cast<int>(presets_.size());
}

auto TextStylePreset::find_preset(const std::string& name) const -> const TextPreset*
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

auto TextStylePreset::set_active(const std::string& name) -> bool
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

auto TextStylePreset::active() const -> const TextPreset&
{
    if (active_index_ >= 0 && static_cast<size_t>(active_index_) < presets_.size())
    {
        return presets_[static_cast<size_t>(active_index_)];
    }
    return default_preset_;
}

void TextStylePreset::register_standard_presets()
{
    TextPreset heading;
    heading.name = "heading";
    heading.type = TextPresetType::kHeading;
    heading.font_family = "sans-serif";
    heading.font_size = 24.0;
    heading.font_weight = 700;
    heading.color_rgba = 0x111111FF;
    add_preset(heading);

    TextPreset body;
    body.name = "body";
    body.type = TextPresetType::kBody;
    body.font_family = "sans-serif";
    body.font_size = 14.0;
    body.font_weight = 400;
    body.color_rgba = 0x333333FF;
    add_preset(body);

    TextPreset callout;
    callout.name = "callout";
    callout.type = TextPresetType::kCallout;
    callout.font_family = "sans-serif";
    callout.font_size = 16.0;
    callout.font_weight = 500;
    callout.color_rgba = 0x0066CCFF;
    add_preset(callout);

    TextPreset code;
    code.name = "code";
    code.type = TextPresetType::kCode;
    code.font_family = "monospace";
    code.font_size = 13.0;
    code.font_weight = 400;
    code.color_rgba = 0x222222FF;
    add_preset(code);
}

} // namespace markamp::canvas
