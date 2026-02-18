/// test_image_object_data.cpp — Unit tests
#include "canvas/ImageObject.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::canvas;

TEST_CASE("ImageFormat: enum values", "[image_object]")
{
    REQUIRE(static_cast<uint8_t>(ImageFormat::kPng) != static_cast<uint8_t>(ImageFormat::kJpeg));
}

TEST_CASE("ImageObject: compiles", "[image_object]")
{
    static_assert(sizeof(ImageObject) > 0);
}
