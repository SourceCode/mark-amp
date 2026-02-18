/// @file ImportPresetRegistry.cpp
/// @brief V9 Phase 42 — ImportPresetRegistry implementation.

#include "ImportPresetRegistry.h"

#include <algorithm>

namespace markamp::core
{

void ImportPresetRegistry::register_preset(ImportPreset preset)
{
    if (preset.preset_id.empty())
    {
        preset.preset_id = "preset_" + std::to_string(presets_.size());
    }
    presets_.push_back(std::move(preset));
}

auto ImportPresetRegistry::remove_preset(const std::string& preset_id) -> bool
{
    auto iter =
        std::remove_if(presets_.begin(),
                       presets_.end(),
                       [&](const ImportPreset& preset) { return preset.preset_id == preset_id; });
    if (iter == presets_.end())
    {
        return false;
    }
    presets_.erase(iter, presets_.end());
    return true;
}

auto ImportPresetRegistry::find_preset(const std::string& preset_id) const -> const ImportPreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            return &preset;
        }
    }
    return nullptr;
}

auto ImportPresetRegistry::preset_count() const -> int
{
    return static_cast<int>(presets_.size());
}

void ImportPresetRegistry::load_defaults()
{
    ImportPreset docx;
    docx.preset_id = "default_docx";
    docx.name = "Word Document Import";
    docx.description = "Import Word documents with formatting";
    docx.target_format = ImportFormat::kDocx;
    docx.options.preserve_formatting = true;
    docx.options.extract_images = true;
    docx.options.convert_tables = true;
    docx.is_default = true;
    presets_.push_back(std::move(docx));

    ImportPreset html;
    html.preset_id = "default_html";
    html.name = "HTML Import";
    html.description = "Import HTML pages as Markdown";
    html.target_format = ImportFormat::kHtml;
    html.options.preserve_formatting = true;
    html.options.extract_images = true;
    presets_.push_back(std::move(html));

    ImportPreset plain;
    plain.preset_id = "default_txt";
    plain.name = "Plain Text Import";
    plain.description = "Import plain text files";
    plain.target_format = ImportFormat::kTxt;
    plain.options.preserve_formatting = false;
    presets_.push_back(std::move(plain));
}

auto ImportPresetRegistry::set_default(const std::string& preset_id) -> bool
{
    // Clear existing default
    for (auto& preset : presets_)
    {
        preset.is_default = false;
    }
    auto* target = find_mut(preset_id);
    if (target == nullptr)
    {
        return false;
    }
    target->is_default = true;
    return true;
}

auto ImportPresetRegistry::default_preset() const -> const ImportPreset*
{
    for (const auto& preset : presets_)
    {
        if (preset.is_default)
        {
            return &preset;
        }
    }
    return nullptr;
}

auto ImportPresetRegistry::mark_used(const std::string& preset_id) -> bool
{
    auto* preset = find_mut(preset_id);
    if (preset == nullptr)
    {
        return false;
    }
    preset->usage_count++;
    return true;
}

auto ImportPresetRegistry::all_presets() const -> std::vector<const ImportPreset*>
{
    std::vector<const ImportPreset*> result;
    result.reserve(presets_.size());
    for (const auto& preset : presets_)
    {
        result.push_back(&preset);
    }
    return result;
}

auto ImportPresetRegistry::presets_for_format(ImportFormat format) const
    -> std::vector<const ImportPreset*>
{
    std::vector<const ImportPreset*> result;
    for (const auto& preset : presets_)
    {
        if (preset.target_format == format)
        {
            result.push_back(&preset);
        }
    }
    return result;
}

void ImportPresetRegistry::clear_all()
{
    presets_.clear();
}

auto ImportPresetRegistry::find_mut(const std::string& preset_id) -> ImportPreset*
{
    for (auto& preset : presets_)
    {
        if (preset.preset_id == preset_id)
        {
            return &preset;
        }
    }
    return nullptr;
}

} // namespace markamp::core
