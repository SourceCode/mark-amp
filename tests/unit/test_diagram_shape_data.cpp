/// test_diagram_shape_data.cpp — Unit tests

#include "canvas/DiagramShapeObject.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("DiagramLibrary: enum values", "[diagram_shape]")
{
    REQUIRE(static_cast<uint8_t>(DiagramLibrary::UML) != static_cast<uint8_t>(DiagramLibrary::BPMN));
}

TEST_CASE("UMLShapeType: enum values", "[diagram_shape]")
{
    REQUIRE(static_cast<uint8_t>(UMLShapeType::Class) != static_cast<uint8_t>(UMLShapeType::Interface));
}

TEST_CASE("BPMNShapeType: enum values", "[diagram_shape]")
{
    REQUIRE(static_cast<uint8_t>(BPMNShapeType::StartEvent) != static_cast<uint8_t>(BPMNShapeType::EndEvent));
}
