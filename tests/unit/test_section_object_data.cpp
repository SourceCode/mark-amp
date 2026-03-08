/// test_section_object_data.cpp
#include "canvas/SectionObject.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("SectionObject: type compiles", "[section_object_data]")
{
    static_assert(sizeof(SectionObject) > 0);
}
