#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

enum class DiagramLibrary : uint8_t
{
    UML,
    BPMN
};

enum class UMLShapeType : uint8_t
{
    Class,
    Interface,
    AbstractClass,
    Actor,
    UseCase,
    Package,
    Component,
    Node,
    Note,
    Lifeline,
    ActivationBar,
    Object
};

enum class BPMNShapeType : uint8_t
{
    StartEvent,
    EndEvent,
    IntermediateEvent,
    Task,
    SubProcess,
    Gateway,
    DataObject,
    DataStore,
    Pool,
    Lane,
    Annotation,
    MessageFlow
};

/// A diagram shape that supports both UML and BPMN notations with
/// compartmented text (e.g. class name / attributes / methods).
class DiagramShapeObject : public CanvasObject
{
public:
    DiagramShapeObject();

    [[nodiscard]] auto library() const -> DiagramLibrary;
    auto set_library(DiagramLibrary lib) -> void;

    [[nodiscard]] auto uml_type() const -> UMLShapeType;
    auto set_uml_type(UMLShapeType type) -> void;

    [[nodiscard]] auto bpmn_type() const -> BPMNShapeType;
    auto set_bpmn_type(BPMNShapeType type) -> void;

    [[nodiscard]] auto compartments() const -> const std::vector<std::string>&;
    auto set_compartments(const std::vector<std::string>& compartments) -> void;

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto fill_color() const -> const CanvasColor&;
    auto set_fill_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;
    auto set_stroke_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    DiagramLibrary library_{DiagramLibrary::UML};
    UMLShapeType uml_type_{UMLShapeType::Class};
    BPMNShapeType bpmn_type_{BPMNShapeType::Task};
    std::vector<std::string> compartments_;
    std::string title_{"Class"};
    double width_{160.0};
    double height_{120.0};
    CanvasColor fill_color_{255, 255, 200, 255};
    CanvasColor stroke_color_{0, 0, 0, 255};
};

} // namespace markamp::canvas
