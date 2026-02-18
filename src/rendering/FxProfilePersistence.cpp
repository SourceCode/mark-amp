// ============================================================================
// File: src/rendering/FxProfilePersistence.cpp
// Phase 28: FX Visual Effects System — FX Profile Serialization
// ============================================================================

#include "FxProfilePersistence.h"

#include <nlohmann/json.hpp>

#include <algorithm>

namespace markamp::rendering
{

// ── JSON helpers for MotionPreset ──

static auto motion_to_json(const MotionPreset& motion) -> nlohmann::json
{
    return {
        {"name", motion.name},
        {"motion_intensity", motion.motion_intensity},
        {"glow_intensity", motion.glow_intensity},
        {"bloom_intensity", motion.bloom_intensity},
        {"chroma_intensity", motion.chroma_intensity},
        {"stiffness", motion.stiffness},
        {"damping", motion.damping},
        {"overshoot", motion.overshoot},
        {"duration_scale", motion.duration_scale},
        {"text_distortion_allowed", motion.text_distortion_allowed},
        {"safe_text_clamp", motion.safe_text_clamp},
    };
}

static auto json_to_motion(const nlohmann::json& json_obj) -> MotionPreset
{
    MotionPreset motion;
    if (json_obj.contains("name"))
    {
        motion.name = json_obj["name"].get<std::string>();
    }
    if (json_obj.contains("motion_intensity"))
    {
        motion.motion_intensity = json_obj["motion_intensity"].get<float>();
    }
    if (json_obj.contains("glow_intensity"))
    {
        motion.glow_intensity = json_obj["glow_intensity"].get<float>();
    }
    if (json_obj.contains("bloom_intensity"))
    {
        motion.bloom_intensity = json_obj["bloom_intensity"].get<float>();
    }
    if (json_obj.contains("chroma_intensity"))
    {
        motion.chroma_intensity = json_obj["chroma_intensity"].get<float>();
    }
    if (json_obj.contains("stiffness"))
    {
        motion.stiffness = json_obj["stiffness"].get<float>();
    }
    if (json_obj.contains("damping"))
    {
        motion.damping = json_obj["damping"].get<float>();
    }
    if (json_obj.contains("overshoot"))
    {
        motion.overshoot = json_obj["overshoot"].get<float>();
    }
    if (json_obj.contains("duration_scale"))
    {
        motion.duration_scale = json_obj["duration_scale"].get<float>();
    }
    if (json_obj.contains("text_distortion_allowed"))
    {
        motion.text_distortion_allowed = json_obj["text_distortion_allowed"].get<bool>();
    }
    if (json_obj.contains("safe_text_clamp"))
    {
        motion.safe_text_clamp = json_obj["safe_text_clamp"].get<bool>();
    }
    return motion;
}

// ── Serialization ──

auto FxProfilePersistence::serialize(const FxPreset& preset) const -> std::string
{
    nlohmann::json json_obj;
    json_obj["name"] = preset.name;
    json_obj["description"] = preset.description;
    json_obj["version"] = preset.version;
    json_obj["is_builtin"] = preset.is_builtin;
    json_obj["motion"] = motion_to_json(preset.motion);

    // Text effects
    nlohmann::json text_json;
    text_json["stroke_enabled"] = preset.text.stroke_enabled;
    text_json["stroke_width"] = preset.text.stroke_width;
    text_json["shadow_enabled"] = preset.text.shadow_enabled;
    text_json["shadow_blur"] = preset.text.shadow_blur;
    text_json["outer_glow_enabled"] = preset.text.outer_glow_enabled;
    text_json["outer_glow_radius"] = preset.text.outer_glow_radius;
    text_json["gradient_fill_enabled"] = preset.text.gradient_fill_enabled;
    text_json["bloom_enabled"] = preset.text.bloom_enabled;
    text_json["bloom_intensity"] = preset.text.bloom_intensity;
    text_json["crt_mode"] = preset.text.crt_mode;
    json_obj["text"] = text_json;

    // Editor effects
    nlohmann::json editor_json;
    editor_json["caret_glow_enabled"] = preset.editor.caret_glow_enabled;
    editor_json["caret_glow_radius"] = preset.editor.caret_glow_radius;
    editor_json["selection_shimmer"] = preset.editor.selection_shimmer;
    editor_json["active_line_glow"] = preset.editor.active_line_glow;
    editor_json["diagnostic_pulse"] = preset.editor.diagnostic_pulse;
    json_obj["editor"] = editor_json;

    // Canvas effects
    nlohmann::json canvas_json;
    canvas_json["object_shadow_blur"] = preset.canvas.object_shadow_blur;
    canvas_json["selection_glow_pulse"] = preset.canvas.selection_glow_pulse;
    canvas_json["connector_neon_intensity"] = preset.canvas.connector_neon_intensity;
    canvas_json["sticky_note_lighting"] = preset.canvas.sticky_note_lighting;
    json_obj["canvas"] = canvas_json;

    return json_obj.dump(2);
}

auto FxProfilePersistence::deserialize(const std::string& json_data) const
    -> std::optional<FxPreset>
{
    try
    {
        auto json_obj = nlohmann::json::parse(json_data);

        FxPreset preset;
        if (json_obj.contains("name"))
        {
            preset.name = json_obj["name"].get<std::string>();
        }
        if (json_obj.contains("description"))
        {
            preset.description = json_obj["description"].get<std::string>();
        }
        if (json_obj.contains("version"))
        {
            preset.version = json_obj["version"].get<uint32_t>();
        }
        if (json_obj.contains("is_builtin"))
        {
            preset.is_builtin = json_obj["is_builtin"].get<bool>();
        }
        if (json_obj.contains("motion"))
        {
            preset.motion = json_to_motion(json_obj["motion"]);
        }

        // Text effects
        if (json_obj.contains("text"))
        {
            const auto& text_json = json_obj["text"];
            if (text_json.contains("stroke_enabled"))
            {
                preset.text.stroke_enabled = text_json["stroke_enabled"].get<bool>();
            }
            if (text_json.contains("shadow_enabled"))
            {
                preset.text.shadow_enabled = text_json["shadow_enabled"].get<bool>();
            }
            if (text_json.contains("outer_glow_enabled"))
            {
                preset.text.outer_glow_enabled = text_json["outer_glow_enabled"].get<bool>();
            }
            if (text_json.contains("gradient_fill_enabled"))
            {
                preset.text.gradient_fill_enabled = text_json["gradient_fill_enabled"].get<bool>();
            }
            if (text_json.contains("bloom_enabled"))
            {
                preset.text.bloom_enabled = text_json["bloom_enabled"].get<bool>();
            }
            if (text_json.contains("bloom_intensity"))
            {
                preset.text.bloom_intensity = text_json["bloom_intensity"].get<float>();
            }
            if (text_json.contains("crt_mode"))
            {
                preset.text.crt_mode = text_json["crt_mode"].get<bool>();
            }
        }

        // Editor effects
        if (json_obj.contains("editor"))
        {
            const auto& editor_json = json_obj["editor"];
            if (editor_json.contains("caret_glow_enabled"))
            {
                preset.editor.caret_glow_enabled = editor_json["caret_glow_enabled"].get<bool>();
            }
            if (editor_json.contains("selection_shimmer"))
            {
                preset.editor.selection_shimmer = editor_json["selection_shimmer"].get<bool>();
            }
            if (editor_json.contains("active_line_glow"))
            {
                preset.editor.active_line_glow = editor_json["active_line_glow"].get<bool>();
            }
            if (editor_json.contains("diagnostic_pulse"))
            {
                preset.editor.diagnostic_pulse = editor_json["diagnostic_pulse"].get<bool>();
            }
        }

        // Canvas effects
        if (json_obj.contains("canvas"))
        {
            const auto& canvas_json = json_obj["canvas"];
            if (canvas_json.contains("selection_glow_pulse"))
            {
                preset.canvas.selection_glow_pulse =
                    canvas_json["selection_glow_pulse"].get<bool>();
            }
            if (canvas_json.contains("connector_neon_intensity"))
            {
                preset.canvas.connector_neon_intensity =
                    canvas_json["connector_neon_intensity"].get<float>();
            }
            if (canvas_json.contains("sticky_note_lighting"))
            {
                preset.canvas.sticky_note_lighting =
                    canvas_json["sticky_note_lighting"].get<bool>();
            }
        }

        return preset;
    }
    catch (const nlohmann::json::exception&)
    {
        return std::nullopt;
    }
}

// ── Profile management ──

auto FxProfilePersistence::save_user_profile(const std::string& profile_name,
                                             const FxPreset& preset) -> void
{
    // Overwrite if exists.
    for (auto& [name, stored_preset] : profiles_)
    {
        if (name == profile_name)
        {
            stored_preset = preset;
            return;
        }
    }
    profiles_.emplace_back(profile_name, preset);
}

auto FxProfilePersistence::load_user_profile(const std::string& profile_name) const
    -> std::optional<FxPreset>
{
    for (const auto& [name, preset] : profiles_)
    {
        if (name == profile_name)
        {
            return preset;
        }
    }
    return std::nullopt;
}

auto FxProfilePersistence::list_user_profiles() const -> std::vector<std::string>
{
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& [name, preset] : profiles_)
    {
        names.push_back(name);
    }
    return names;
}

auto FxProfilePersistence::delete_user_profile(const std::string& profile_name) -> bool
{
    auto iter = std::find_if(profiles_.begin(),
                             profiles_.end(),
                             [&](const auto& pair) { return pair.first == profile_name; });
    if (iter != profiles_.end())
    {
        profiles_.erase(iter);
        return true;
    }
    return false;
}

auto FxProfilePersistence::has_profile(const std::string& profile_name) const -> bool
{
    return std::any_of(profiles_.begin(),
                       profiles_.end(),
                       [&](const auto& pair) { return pair.first == profile_name; });
}

auto FxProfilePersistence::profile_count() const noexcept -> std::size_t
{
    return profiles_.size();
}

// ── Bulk operations ──

auto FxProfilePersistence::export_all() const -> std::string
{
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& [name, preset] : profiles_)
    {
        auto json_str = serialize(preset);
        auto json_obj = nlohmann::json::parse(json_str);
        json_obj["_profile_name"] = name;
        arr.push_back(std::move(json_obj));
    }
    return arr.dump(2);
}

auto FxProfilePersistence::import_all(const std::string& json_data) -> int32_t
{
    try
    {
        auto arr = nlohmann::json::parse(json_data);
        if (!arr.is_array())
        {
            return 0;
        }

        int32_t count = 0;
        for (const auto& item : arr)
        {
            auto json_str = item.dump();
            auto preset = deserialize(json_str);
            if (preset.has_value())
            {
                std::string profile_name = preset->name;
                if (item.contains("_profile_name"))
                {
                    profile_name = item["_profile_name"].get<std::string>();
                }
                save_user_profile(profile_name, *preset);
                ++count;
            }
        }
        return count;
    }
    catch (const nlohmann::json::exception&)
    {
        return 0;
    }
}

auto FxProfilePersistence::clear_all() -> void
{
    profiles_.clear();
}

} // namespace markamp::rendering
