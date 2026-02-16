/// FxPresetRegistry.h — Phase 52: Preset System + Theme Binding
///
/// Manages named effect presets that can be bound to themes for atomic
/// switching. Supports built-in, user-created, and imported presets.

#pragma once

#include "FxMotionPreset.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::rendering
{

/// A complete FX preset with motion, text, and UI effect settings.
struct FxPreset
{
    std::string name;
    std::string description;
    uint32_t version{1};
    bool is_builtin{false};

    MotionPreset motion;
    core::TextEffects text;
    core::UiElementEffects ui_elements;
    core::EditorEffects editor;
    core::CanvasEffects canvas;

    auto operator==(const FxPreset&) const -> bool = default;
};

/// Registry for managing FX presets with theme binding support.
class FxPresetRegistry
{
public:
    explicit FxPresetRegistry(core::EventBus& event_bus);

    /// Register a preset. Overwrites if name already exists.
    void register_preset(const FxPreset& preset);

    /// Get a preset by name.
    [[nodiscard]] auto get_preset(const std::string& preset_name) const -> std::optional<FxPreset>;

    /// Remove a preset by name. Returns true if found and removed.
    auto remove_preset(const std::string& preset_name) -> bool;

    /// List all registered preset names.
    [[nodiscard]] auto list_presets() const -> std::vector<std::string>;

    /// Total number of registered presets.
    [[nodiscard]] auto preset_count() const noexcept -> std::size_t;

    /// Check if a preset exists.
    [[nodiscard]] auto has_preset(const std::string& preset_name) const -> bool;

    /// Import a preset from serialized data (JSON string).
    auto import_preset(const std::string& json_data) -> bool;

    /// Export a preset to serialized data (JSON string).
    [[nodiscard]] auto export_preset(const std::string& preset_name) const
        -> std::optional<std::string>;

    /// Bind a preset to a theme (by theme ID). When theme is applied,
    /// the bound preset is automatically activated.
    void bind_to_theme(const std::string& preset_name, const std::string& theme_id);

    /// Get the preset bound to a theme (empty if none).
    [[nodiscard]] auto preset_for_theme(const std::string& theme_id) const
        -> std::optional<std::string>;

    /// Initialize built-in presets.
    void initialize_builtins();

private:
    core::EventBus& event_bus_;
    std::vector<FxPreset> presets_;

    /// theme_id -> preset_name
    std::unordered_map<std::string, std::string> theme_bindings_;
};

} // namespace markamp::rendering
