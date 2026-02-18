/// test_section_object_data.cpp — Unit tests
#include "canvas/SectionObject.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("SectionObject: compiles", "[section_object]")
{
    static_assert(sizeof(SectionObject) > 0);
}
