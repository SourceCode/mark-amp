#pragma once

#include "canvas/DiagramShapeObject.h"

#include <functional>
#include <string>
#include <vector>

#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/graphics.h>
#include <wx/pen.h>

class wxGraphicsContext;

namespace markamp::canvas
{

/// Description of a shape template in the diagram library panel.
struct DiagramShapeTemplate
{
    std::string name;
    DiagramLibrary library;
    UMLShapeType uml_type;
    BPMNShapeType bpmn_type;
    double default_width;
    double default_height;
};

/// A panel that displays available diagram shapes for a given library (UML or BPMN).
class DiagramLibraryPanel
{
public:
    using OnShapeSelected = std::function<void(const DiagramShapeTemplate& tmpl)>;

    auto set_library(DiagramLibrary library) -> void;
    [[nodiscard]] auto active_library() const -> DiagramLibrary;

    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;
    auto set_on_shape_selected(OnShapeSelected cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    /// Get all shape templates for the active library.
    [[nodiscard]] auto get_templates() const -> std::vector<DiagramShapeTemplate>;

private:
    /// Cached item layout for hit testing.
    struct CachedItem
    {
        double x;
        double y;
        double width;
        double height;
        size_t template_index;
    };

    DiagramLibrary active_library_{DiagramLibrary::UML};
    bool visible_{false};
    OnShapeSelected on_shape_selected_;
    mutable std::vector<CachedItem> cached_items_;
};

} // namespace markamp::canvas
