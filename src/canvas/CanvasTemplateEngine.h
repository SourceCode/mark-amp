#pragma once

/// @file CanvasTemplateEngine.h
/// @brief Phase 14: Board template registration and instantiation.
///
/// Manages canvas board templates — both built-in and extension-contributed.
/// Templates declare a category, tags, and serialized board JSON that
/// can be instantiated onto the canvas with unique IDs and position offsets.

#include "core/EventBus.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// A board template that can be applied to the canvas.
struct CanvasTemplate
{
    std::string template_id;  ///< Unique identifier
    std::string name;         ///< Display name
    std::string description;  ///< Short description
    std::string category;     ///< "brainstorming", "planning", "retrospective", etc.
    std::string extension_id; ///< Empty for built-in templates
    std::string board_json;   ///< Serialized board state
    std::vector<std::string> tags;
    std::string preview_data; ///< Thumbnail/preview description
    bool is_built_in{false};

    /// Whether this template was contributed by an extension.
    [[nodiscard]] auto is_extension_template() const noexcept -> bool
    {
        return !extension_id.empty();
    }

    /// Whether the template has tags.
    [[nodiscard]] auto has_tags() const noexcept -> bool
    {
        return !tags.empty();
    }

    // ── Round 2 Batch 8 (#71-73) ──────────────────────────────────

    /// (#71) Whether preview data is available.
    [[nodiscard]] auto has_preview() const noexcept -> bool
    {
        return !preview_data.empty();
    }

    /// (#72) Whether a description is provided.
    [[nodiscard]] auto has_description() const noexcept -> bool
    {
        return !description.empty();
    }

    /// (#73) Number of tags.
    [[nodiscard]] auto tag_count() const noexcept -> size_t
    {
        return tags.size();
    }
};

/// Result of applying a template.
struct TemplateApplicationResult
{
    bool success{false};
    std::string error_message;
    std::string applied_template_id;
    int objects_created{0}; ///< Number of objects instantiated

    /// Whether the template application failed.
    [[nodiscard]] auto failed() const noexcept -> bool
    {
        return !success;
    }

    // ── Round 2 Batch 8 (#74-75) ──────────────────────────────────

    /// (#74) Whether there's an error message.
    [[nodiscard]] auto has_error() const noexcept -> bool
    {
        return !error_message.empty();
    }

    /// (#75) Whether no objects were created.
    [[nodiscard]] auto is_empty_apply() const noexcept -> bool
    {
        return objects_created == 0;
    }
};

/// Engine for managing and applying canvas board templates.
///
/// Templates are registered from extension manifests and built-in
/// sources. They can be filtered by category/tags and applied to
/// create new objects on the canvas.
class CanvasTemplateEngine
{
public:
    explicit CanvasTemplateEngine(core::EventBus& event_bus);

    // ── Registration ──────────────────────────────────────────────

    /// Register a board template.
    auto register_template(const CanvasTemplate& tmpl) -> bool;

    /// Unregister a template by ID.
    auto unregister_template(const std::string& template_id) -> bool;

    /// Unregister all templates from a specific extension.
    auto unregister_extension_templates(const std::string& extension_id) -> size_t;

    // ── Application ───────────────────────────────────────────────

    /// Apply a template to the canvas (returns a description of what was created).
    auto apply_template(const std::string& template_id,
                        double offset_x = 0.0,
                        double offset_y = 0.0) -> TemplateApplicationResult;

    // ── Query ─────────────────────────────────────────────────────

    /// Find a template by ID.
    [[nodiscard]] auto find_template(const std::string& template_id) const -> const CanvasTemplate*;

    /// All registered templates.
    [[nodiscard]] auto list_templates() const
        -> const std::unordered_map<std::string, CanvasTemplate>&;

    /// Templates in a given category.
    [[nodiscard]] auto templates_in_category(const std::string& category) const
        -> std::vector<const CanvasTemplate*>;

    /// Search templates by name or tag substring.
    [[nodiscard]] auto search_templates(const std::string& query) const
        -> std::vector<const CanvasTemplate*>;

    /// All unique categories.
    [[nodiscard]] auto template_categories() const -> std::vector<std::string>;

    /// Whether a template ID is registered.
    [[nodiscard]] auto is_registered(const std::string& template_id) const -> bool;

    /// Number of registered templates.
    [[nodiscard]] auto template_count() const -> size_t;

    /// (#93) Count only built-in templates.
    [[nodiscard]] auto builtin_count() const -> std::size_t;

    // ── Cleanup ───────────────────────────────────────────────────

    auto clear() -> void;

private:
    core::EventBus& event_bus_;
    std::unordered_map<std::string, CanvasTemplate> templates_;
};

} // namespace markamp::canvas
