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
    wxGraphicsContext& /*gc*/, double /*x*/, double /*y*/, double /*w*/, double /*h*/) -> void
{
    // Stub — rendering deferred to UI integration phase.
}

auto DiagramLibraryPanel::handle_click(double /*x*/, double /*y*/) -> bool
{
    // Stub — hit testing deferred.
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
