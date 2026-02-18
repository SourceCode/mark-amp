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
};

/// Result of applying a template.
struct TemplateApplicationResult
{
    bool success{false};
    std::string error_message;
    std::string applied_template_id;
    int objects_created{0}; ///< Number of objects instantiated
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

    // ── Cleanup ───────────────────────────────────────────────────

    auto clear() -> void;

private:
    core::EventBus& event_bus_;
    std::unordered_map<std::string, CanvasTemplate> templates_;
};

} // namespace markamp::canvas
