/// test_diagram_shape_object.cpp — Unit tests
#include "canvas/DiagramShapeObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("DiagramLibrary: enum values", "[diagram_shape_object]")
{
    REQUIRE(static_cast<int>(DiagramLibrary::UML) != static_cast<int>(DiagramLibrary::BPMN));
}

TEST_CASE("UMLShapeType: enum values", "[diagram_shape_object]")
{
    REQUIRE(static_cast<int>(UMLShapeType::Class) != static_cast<int>(UMLShapeType::Interface));
}

TEST_CASE("BPMNShapeType: enum values", "[diagram_shape_object]")
{
    REQUIRE(static_cast<int>(BPMNShapeType::StartEvent) != static_cast<int>(BPMNShapeType::EndEvent));
}

TEST_CASE("DiagramLibrary: compiles", "[diagram_shape_object]")
{
    static_assert(sizeof(DiagramLibrary) > 0);
}

TEST_CASE("UMLShapeType: compiles", "[diagram_shape_object]")
{
    static_assert(sizeof(UMLShapeType) > 0);
}

TEST_CASE("BPMNShapeType: compiles", "[diagram_shape_object]")
{
    static_assert(sizeof(BPMNShapeType) > 0);
}
