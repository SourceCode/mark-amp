// ============================================================================
// File: src/canvas/CanvasStylePresetService.h
// Phase 12: Canvas Advanced Objects — style presets and eyedropper
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/SelectionManager.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Visual style properties that can be saved as a preset.
struct StylePreset
{
    std::string name;
    std::string category; ///< e.g. "Sticky Notes", "Shapes", "Custom"

    // ── Fill & Border ──
    std::optional<CanvasColor> fill_color;
    std::optional<CanvasColor> border_color;
    std::optional<double> border_width;
    std::optional<double> corner_radius;
    std::optional<double> opacity;

    // ── Text ──
    std::optional<std::string> font_family;
    std::optional<double> font_size;
    std::optional<CanvasColor> text_color;
    std::optional<bool> bold;
    std::optional<bool> italic;

    // ── Shadow ──
    std::optional<bool> has_shadow;
    std::optional<double> shadow_blur;
    std::optional<CanvasColor> shadow_color;
};

/// Result of applying a style preset.
struct StyleApplyResult
{
    bool success{false};
    size_t objects_modified{0};
    std::string error_message;
};

/// Built-in preset categories.
enum class PresetCategory : uint8_t
{
    kStickyNotes,
    kShapes,
    kConnectors,
    kText,
    kCustom
};

/// Canvas style preset service.
///
/// Provides:
///   - Named style presets with fill, border, text, and shadow properties
///   - Apply preset to single or multiple objects
///   - Create preset from an existing object's style
///   - Eyedropper: copy style from one object to another
///   - Built-in preset library with common color schemes
class CanvasStylePresetService
{
public:
    explicit CanvasStylePresetService(Board& board);

    // ── Preset Management ─────────────────────────────────────────

    /// Add a custom preset to the library.
    auto add_preset(const StylePreset& preset) -> void;

    /// Remove a preset by name.
    auto remove_preset(const std::string& name) -> bool;

    /// Get all available presets.
    [[nodiscard]] auto presets() const -> const std::vector<StylePreset>&;

    /// Get presets filtered by category.
    [[nodiscard]] auto presets_in_category(const std::string& category) const
        -> std::vector<StylePreset>;

    /// Find a preset by name.
    [[nodiscard]] auto find_preset(const std::string& name) const -> const StylePreset*;

    // ── Apply Preset ──────────────────────────────────────────────

    /// Apply a preset to a single object.
    auto apply_preset(ObjectId obj_id, const StylePreset& preset) -> StyleApplyResult;

    /// Apply a preset to multiple objects.
    auto apply_preset_to_selection(const std::vector<ObjectId>& ids, const StylePreset& preset)
        -> StyleApplyResult;

    // ── Create Preset from Object ─────────────────────────────────

    /// Extract a style preset from an existing object.
    [[nodiscard]] auto create_preset_from_object(ObjectId obj_id,
                                                 const std::string& preset_name) const
        -> StylePreset;

    // ── Eyedropper ────────────────────────────────────────────────

    /// Copy style from source object to target object.
    auto eyedropper(ObjectId source_id, ObjectId target_id) -> StyleApplyResult;

    /// Copy style from source object to all objects in the list.
    auto eyedropper_to_selection(ObjectId source_id, const std::vector<ObjectId>& target_ids)
        -> StyleApplyResult;

    // ── Built-in Presets ──────────────────────────────────────────

    /// Load the built-in preset library.
    auto load_builtin_presets() -> void;

    /// Get the number of presets.
    [[nodiscard]] auto preset_count() const -> size_t;

    /// Available category names.
    [[nodiscard]] static auto category_names() -> std::vector<std::string>;

private:
    Board& board_;
    std::vector<StylePreset> presets_;

    /// Apply style properties from a preset to a concrete object.
    auto apply_style_to_object(CanvasObject& obj, const StylePreset& preset) -> void;

    /// Extract style properties from a concrete object.
    [[nodiscard]] static auto extract_style(const CanvasObject& obj) -> StylePreset;
};

} // namespace markamp::canvas
