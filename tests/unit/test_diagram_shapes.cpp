#include "canvas/DiagramLibraryPanel.h"
#include "canvas/DiagramShapeObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("DiagramShape: UML Class with compartments", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_library(DiagramLibrary::UML);
    shape.set_uml_type(UMLShapeType::Class);
    shape.set_title("Person");
    shape.set_compartments({"+ name: string", "- age: int", "+ getName(): string"});

    REQUIRE(shape.library() == DiagramLibrary::UML);
    REQUIRE(shape.uml_type() == UMLShapeType::Class);
    REQUIRE(shape.title() == "Person");
    REQUIRE(shape.compartments().size() == 3);
}

TEST_CASE("DiagramShape: UML Actor dimensions", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_uml_type(UMLShapeType::Actor);
    shape.set_dimensions(60, 100);

    REQUIRE(shape.width() == 60.0);
    REQUIRE(shape.height() == 100.0);
    const auto bounds = shape.local_bounds();
    REQUIRE(bounds.max_x == 60.0);
    REQUIRE(bounds.max_y == 100.0);
}

TEST_CASE("DiagramShape: BPMN Task", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_library(DiagramLibrary::BPMN);
    shape.set_bpmn_type(BPMNShapeType::Task);
    shape.set_title("Review Document");

    REQUIRE(shape.library() == DiagramLibrary::BPMN);
    REQUIRE(shape.bpmn_type() == BPMNShapeType::Task);
    REQUIRE(shape.title() == "Review Document");
}

TEST_CASE("DiagramShape: BPMN Gateway", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_library(DiagramLibrary::BPMN);
    shape.set_bpmn_type(BPMNShapeType::Gateway);
    shape.set_dimensions(60, 60);

    REQUIRE(shape.bpmn_type() == BPMNShapeType::Gateway);
    REQUIRE(shape.width() == 60.0);
}

TEST_CASE("DiagramLibraryPanel: UML has 12 templates", "[diagram_shapes]")
{
    DiagramLibraryPanel panel;
    panel.set_library(DiagramLibrary::UML);
    const auto templates = panel.get_templates();
    REQUIRE(templates.size() == 12);
}

TEST_CASE("DiagramLibraryPanel: BPMN has 12 templates", "[diagram_shapes]")
{
    DiagramLibraryPanel panel;
    panel.set_library(DiagramLibrary::BPMN);
    const auto templates = panel.get_templates();
    REQUIRE(templates.size() == 12);
}

TEST_CASE("DiagramShape: JSON round-trip", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_title("OrderService");
    shape.set_compartments({"+ process()", "- validate()"});

    const auto json = shape.to_json();
    REQUIRE(json.find("\"title\":\"OrderService\"") != std::string::npos);
    REQUIRE(json.find("\"compartments\":[") != std::string::npos);
    REQUIRE(json.find("\"+ process()\"") != std::string::npos);
}

TEST_CASE("DiagramShape: clone", "[diagram_shapes]")
{
    DiagramShapeObject shape;
    shape.set_library(DiagramLibrary::BPMN);
    shape.set_bpmn_type(BPMNShapeType::Gateway);
    shape.set_title("Decision");
    shape.set_fill_color({200, 200, 255, 255});

    const auto cloned = shape.clone();
    const auto* copy = dynamic_cast<const DiagramShapeObject*>(cloned.get());
    REQUIRE(copy != nullptr);
    REQUIRE(copy->library() == DiagramLibrary::BPMN);
    REQUIRE(copy->bpmn_type() == BPMNShapeType::Gateway);
    REQUIRE(copy->title() == "Decision");
    REQUIRE(copy->fill_color().b == 255);
}
