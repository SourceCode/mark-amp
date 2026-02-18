// ============================================================================
// File: src/canvas/CanvasStylePresetService.cpp
// Phase 12: Canvas Advanced Objects — style presets and eyedropper
// ============================================================================
#include "canvas/CanvasStylePresetService.h"

#include "canvas/StickyNote.h"
#include "canvas/TextBox.h"

#include <algorithm>

namespace markamp::canvas
{

CanvasStylePresetService::CanvasStylePresetService(Board& board)
    : board_(board)
{
    load_builtin_presets();
}

// ── Preset Management ─────────────────────────────────────────────

auto CanvasStylePresetService::add_preset(const StylePreset& preset) -> void
{
    presets_.push_back(preset);
}

auto CanvasStylePresetService::remove_preset(const std::string& name) -> bool
{
    const auto iter = std::ranges::find_if(
        presets_, [&](const StylePreset& preset) { return preset.name == name; });
    if (iter == presets_.end())
    {
        return false;
    }
    presets_.erase(iter);
    return true;
}

auto CanvasStylePresetService::presets() const -> const std::vector<StylePreset>&
{
    return presets_;
}

auto CanvasStylePresetService::presets_in_category(const std::string& category) const
    -> std::vector<StylePreset>
{
    std::vector<StylePreset> result;
    for (const auto& preset : presets_)
    {
        if (preset.category == category)
        {
            result.push_back(preset);
        }
    }
    return result;
}

auto CanvasStylePresetService::find_preset(const std::string& name) const -> const StylePreset*
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

// ── Apply Preset ──────────────────────────────────────────────────

auto CanvasStylePresetService::apply_preset(ObjectId obj_id, const StylePreset& preset)
    -> StyleApplyResult
{
    auto* obj = board_.get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return {false, 0, "Object not found"};
    }

    apply_style_to_object(*obj, preset);
    board_.mark_dirty();
    return {true, 1, ""};
}

auto CanvasStylePresetService::apply_preset_to_selection(const std::vector<ObjectId>& ids,
                                                         const StylePreset& preset)
    -> StyleApplyResult
{
    size_t modified = 0;
    for (const auto obj_id : ids)
    {
        auto* obj = board_.get_object_mut(obj_id);
        if (obj != nullptr)
        {
            apply_style_to_object(*obj, preset);
            ++modified;
        }
    }

    if (modified > 0)
    {
        board_.mark_dirty();
    }
    return {modified > 0, modified, modified == 0 ? "No objects found" : ""};
}

// ── Create Preset from Object ─────────────────────────────────────

auto CanvasStylePresetService::create_preset_from_object(ObjectId obj_id,
                                                         const std::string& preset_name) const
    -> StylePreset
{
    const auto* obj = board_.get_object(obj_id);
    if (obj == nullptr)
    {
        StylePreset empty;
        empty.name = preset_name;
        empty.category = "Custom";
        return empty;
    }

    auto preset = extract_style(*obj);
    preset.name = preset_name;
    preset.category = "Custom";
    return preset;
}

// ── Eyedropper ────────────────────────────────────────────────────

auto CanvasStylePresetService::eyedropper(ObjectId source_id, ObjectId target_id)
    -> StyleApplyResult
{
    const auto* source = board_.get_object(source_id);
    if (source == nullptr)
    {
        return {false, 0, "Source object not found"};
    }

    auto preset = extract_style(*source);
    return apply_preset(target_id, preset);
}

auto CanvasStylePresetService::eyedropper_to_selection(ObjectId source_id,
                                                       const std::vector<ObjectId>& target_ids)
    -> StyleApplyResult
{
    const auto* source = board_.get_object(source_id);
    if (source == nullptr)
    {
        return {false, 0, "Source object not found"};
    }

    auto preset = extract_style(*source);
    return apply_preset_to_selection(target_ids, preset);
}

// ── Built-in Presets ──────────────────────────────────────────────

auto CanvasStylePresetService::load_builtin_presets() -> void
{
    // Sticky Note color presets
    presets_.push_back({"Yellow Sticky",
                        "Sticky Notes",
                        CanvasColor{255, 245, 157, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{66, 66, 66, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Green Sticky",
                        "Sticky Notes",
                        CanvasColor{200, 230, 201, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{33, 33, 33, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Blue Sticky",
                        "Sticky Notes",
                        CanvasColor{187, 222, 251, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{33, 33, 33, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Pink Sticky",
                        "Sticky Notes",
                        CanvasColor{248, 187, 208, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{33, 33, 33, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Orange Sticky",
                        "Sticky Notes",
                        CanvasColor{255, 224, 178, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{33, 33, 33, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    // Shape presets
    presets_.push_back({"Blue Shape",
                        "Shapes",
                        CanvasColor{66, 133, 244, 255},
                        CanvasColor{25, 103, 210, 255},
                        2.0,
                        4.0,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{255, 255, 255, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Red Shape",
                        "Shapes",
                        CanvasColor{234, 67, 53, 255},
                        CanvasColor{197, 34, 31, 255},
                        2.0,
                        4.0,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{255, 255, 255, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Green Shape",
                        "Shapes",
                        CanvasColor{52, 168, 83, 255},
                        CanvasColor{24, 128, 56, 255},
                        2.0,
                        4.0,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        CanvasColor{255, 255, 255, 255},
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    // Text presets
    presets_.push_back({"Heading",
                        "Text",
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::string{"Inter"},
                        24.0,
                        CanvasColor{33, 33, 33, 255},
                        true,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});

    presets_.push_back({"Body",
                        "Text",
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::string{"Inter"},
                        14.0,
                        CanvasColor{66, 66, 66, 255},
                        false,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        std::nullopt});
}

auto CanvasStylePresetService::preset_count() const -> size_t
{
    return presets_.size();
}

auto CanvasStylePresetService::category_names() -> std::vector<std::string>
{
    return {"Sticky Notes", "Shapes", "Connectors", "Text", "Custom"};
}

// ── Private Helpers ───────────────────────────────────────────────

auto CanvasStylePresetService::apply_style_to_object(CanvasObject& obj, const StylePreset& preset)
    -> void
{
    if (preset.opacity.has_value())
    {
        obj.set_opacity(*preset.opacity);
    }

    // Type-specific style application via the object's own properties
    // Fill color, border, font etc. are applied through the object's
    // generic property interface when available
    if (preset.fill_color.has_value())
    {
        obj.set_custom_color(*preset.fill_color);
    }
}

auto CanvasStylePresetService::extract_style(const CanvasObject& obj) -> StylePreset
{
    StylePreset preset;
    preset.opacity = obj.opacity();
    preset.fill_color = obj.custom_color();
    return preset;
}

} // namespace markamp::canvas
