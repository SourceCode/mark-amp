#pragma once

/// @file CanvasAppManifest.h
/// @brief V8 Phase 8: Defines the canvas-app contribution model.
///
/// Extensions declare canvas capabilities — tools, widgets, templates,
/// inspectors, and actions — that are validated by schema and registered
/// at activation time.

#include <string>
#include <vector>

namespace markamp::core
{

/// A custom canvas tool contributed by an extension (e.g. laser pointer, stamp).
struct CanvasToolContribution
{
    std::string tool_id;
    std::string label;
    std::string icon;     ///< Icon path (relative to extension root)
    std::string category; ///< Toolbar category: "draw", "annotate", "navigate"
    int priority{0};      ///< Ordering hint within category
};

/// A data-connected widget that can be embedded on the canvas.
struct CanvasWidgetContribution
{
    std::string widget_id;
    std::string label;
    std::string description;
    std::string icon;
    std::string provider_id;    ///< Data provider identifier
    std::string default_config; ///< JSON default configuration
    int default_width{200};
    int default_height{150};
    bool resizable{true};
};

/// A board template contributed by an extension.
struct CanvasTemplateContribution
{
    std::string template_id;
    std::string label;
    std::string description;
    std::string category;   ///< e.g. "brainstorming", "planning", "retrospective"
    std::string source;     ///< "builtin", "workspace", "extension"
    std::string board_json; ///< Serialized template board state
};

/// A custom inspector panel contributed by an extension.
struct CanvasInspectorContribution
{
    std::string inspector_id;
    std::string label;
    std::string target_object_type; ///< Which object type this inspects (or "*" for all)
    int priority{0};
};

/// A contextual action contributed by an extension (right-click, context menus).
struct CanvasActionContribution
{
    std::string action_id;
    std::string label;
    std::string icon;
    std::string context; ///< "selection", "object", "board", "global"
    std::string when;    ///< Context expression, e.g. "objectType == 'sticky'"
    int priority{0};
};

/// Aggregates all canvas contribution points from a single extension.
struct CanvasAppManifest
{
    std::vector<CanvasToolContribution> tools;
    std::vector<CanvasWidgetContribution> widgets;
    std::vector<CanvasTemplateContribution> templates;
    std::vector<CanvasInspectorContribution> inspectors;
    std::vector<CanvasActionContribution> actions;

    /// Required permission scopes (validated against ExtensionSandbox).
    std::vector<std::string> required_scopes;

    /// Minimum MarkAmp API version this app targets.
    std::string min_api_version;

    /// Whether this app has any canvas contributions.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return tools.empty() && widgets.empty() && templates.empty() && inspectors.empty() &&
               actions.empty();
    }

    /// Total number of contribution points.
    [[nodiscard]] auto contribution_count() const -> size_t
    {
        return tools.size() + widgets.size() + templates.size() + inspectors.size() +
               actions.size();
    }
};

} // namespace markamp::core
