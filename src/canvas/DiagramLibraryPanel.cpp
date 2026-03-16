#include "DiagramLibraryPanel.h"

namespace markamp::canvas
{

auto DiagramLibraryPanel::set_library(DiagramLibrary library) -> void
{
    active_library_ = library;
}

auto DiagramLibraryPanel::active_library() const -> DiagramLibrary
{
    return active_library_;
}

auto DiagramLibraryPanel::render(
    wxGraphicsContext& gc, double x, double y, double w, double /*h*/) -> void
{
    if (!visible_)
    {
        return;
    }

    // Layout constants.
    constexpr double kItemWidth = 120.0;
    constexpr double kItemHeight = 60.0;
    constexpr double kPadding = 8.0;
    constexpr double kHeaderHeight = 24.0;

    const auto templates = get_templates();
    const auto lib_name = (active_library_ == DiagramLibrary::UML) ? "UML Shapes" : "BPMN Shapes";

    // Draw category header.
    gc.SetFont(wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD), wxColour(80, 80, 80));
    gc.DrawText(lib_name, x + kPadding, y + 4.0);

    // Calculate columns that fit the width.
    const int cols = std::max(1, static_cast<int>((w - kPadding * 2.0) / (kItemWidth + kPadding)));

    // Draw template items in a grid.
    cached_items_.clear();
    double cur_y = y + kHeaderHeight + kPadding;
    for (size_t idx = 0; idx < templates.size(); ++idx)
    {
        const int col = static_cast<int>(idx) % cols;
        const int row = static_cast<int>(idx) / cols;
        const double item_x = x + kPadding + static_cast<double>(col) * (kItemWidth + kPadding);
        const double item_y = cur_y + static_cast<double>(row) * (kItemHeight + kPadding);

        // Draw item background.
        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(245, 245, 250))));
        gc.SetPen(gc.CreatePen(wxPen(wxColour(200, 200, 210), 1)));
        gc.DrawRoundedRectangle(item_x, item_y, kItemWidth, kItemHeight, 4.0);

        // Draw shape preview (a miniature representation based on default size).
        const auto& tmpl = templates[idx];
        const double preview_scale = std::min(
            (kItemWidth - 20.0) / tmpl.default_width,
            (kItemHeight - 24.0) / tmpl.default_height) * 0.7;
        const double preview_w = tmpl.default_width * preview_scale;
        const double preview_h = tmpl.default_height * preview_scale;
        const double preview_x = item_x + (kItemWidth - preview_w) / 2.0;
        const double preview_y = item_y + 4.0;

        gc.SetBrush(gc.CreateBrush(wxBrush(wxColour(100, 140, 200, 60))));
        gc.SetPen(gc.CreatePen(wxPen(wxColour(80, 120, 180), 1)));
        gc.DrawRoundedRectangle(preview_x, preview_y, preview_w, preview_h, 2.0);

        // Draw template name.
        gc.SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), wxColour(60, 60, 60));
        gc.DrawText(tmpl.name, item_x + 4.0, item_y + kItemHeight - 16.0);

        // Cache the hit region for click detection.
        cached_items_.push_back({item_x, item_y, kItemWidth, kItemHeight, idx});
    }
}

auto DiagramLibraryPanel::handle_click(double x, double y) -> bool
{
    if (!visible_)
    {
        return false;
    }

    const auto templates = get_templates();
    for (const auto& item : cached_items_)
    {
        if (x >= item.x && x <= item.x + item.width &&
            y >= item.y && y <= item.y + item.height)
        {
            if (item.template_index < templates.size() && on_shape_selected_)
            {
                on_shape_selected_(templates[item.template_index]);
            }
            return true;
        }
    }
    return false;
}

auto DiagramLibraryPanel::set_on_shape_selected(OnShapeSelected cb) -> void
{
    on_shape_selected_ = std::move(cb);
}

auto DiagramLibraryPanel::is_visible() const -> bool
{
    return visible_;
}
auto DiagramLibraryPanel::set_visible(bool visible) -> void
{
    visible_ = visible;
}

auto DiagramLibraryPanel::get_templates() const -> std::vector<DiagramShapeTemplate>
{
    if (active_library_ == DiagramLibrary::UML)
    {
        return {
            {"Class", DiagramLibrary::UML, UMLShapeType::Class, BPMNShapeType::Task, 160, 120},
            {"Interface",
             DiagramLibrary::UML,
             UMLShapeType::Interface,
             BPMNShapeType::Task,
             160,
             100},
            {"Abstract Class",
             DiagramLibrary::UML,
             UMLShapeType::AbstractClass,
             BPMNShapeType::Task,
             160,
             120},
            {"Actor", DiagramLibrary::UML, UMLShapeType::Actor, BPMNShapeType::Task, 60, 100},
            {"Use Case", DiagramLibrary::UML, UMLShapeType::UseCase, BPMNShapeType::Task, 140, 80},
            {"Package", DiagramLibrary::UML, UMLShapeType::Package, BPMNShapeType::Task, 200, 140},
            {"Component",
             DiagramLibrary::UML,
             UMLShapeType::Component,
             BPMNShapeType::Task,
             160,
             100},
            {"Node", DiagramLibrary::UML, UMLShapeType::Node, BPMNShapeType::Task, 120, 100},
            {"Note", DiagramLibrary::UML, UMLShapeType::Note, BPMNShapeType::Task, 140, 80},
            {"Lifeline", DiagramLibrary::UML, UMLShapeType::Lifeline, BPMNShapeType::Task, 80, 300},
            {"Activation Bar",
             DiagramLibrary::UML,
             UMLShapeType::ActivationBar,
             BPMNShapeType::Task,
             20,
             100},
            {"Object", DiagramLibrary::UML, UMLShapeType::Object, BPMNShapeType::Task, 140, 60}};
    }

    return {
        {"Start Event",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::StartEvent,
         40,
         40},
        {"End Event", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::EndEvent, 40, 40},
        {"Intermediate Event",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::IntermediateEvent,
         40,
         40},
        {"Task", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::Task, 140, 80},
        {"Sub-Process",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::SubProcess,
         180,
         100},
        {"Gateway", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::Gateway, 60, 60},
        {"Data Object",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::DataObject,
         60,
         80},
        {"Data Store", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::DataStore, 80, 60},
        {"Pool", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::Pool, 600, 300},
        {"Lane", DiagramLibrary::BPMN, UMLShapeType::Class, BPMNShapeType::Lane, 580, 100},
        {"Annotation",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::Annotation,
         120,
         60},
        {"Message Flow",
         DiagramLibrary::BPMN,
         UMLShapeType::Class,
         BPMNShapeType::MessageFlow,
         100,
         20}};
}

} // namespace markamp::canvas
