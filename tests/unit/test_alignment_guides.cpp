/// test_alignment_guides.cpp — Unit tests

#include "canvas/AlignmentGuides.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("AlignmentGuides: default construction", "[alignment_guides]")
{
    AlignmentGuides guides;
    (void)guides;
}
