/// test_alignment_guides.cpp
#include "canvas/AlignmentGuides.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;
TEST_CASE("AlignmentGuides: type compiles", "[alignment_guides]")
{
    static_assert(sizeof(AlignmentGuides) > 0);
}
