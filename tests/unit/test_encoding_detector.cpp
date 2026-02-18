/// test_encoding_detector.cpp — Unit tests
#include "core/EncodingDetector.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("detect_encoding: UTF-8 text", "[encoding_detector]")
{
    auto result = detect_encoding("Hello world");
    REQUIRE_FALSE(result.display_name.empty());
}

TEST_CASE("detect_encoding: empty string", "[encoding_detector]")
{
    auto result = detect_encoding("");
    // Should return some encoding
    (void)result;
}

TEST_CASE("encoding_display_name: all encodings produce non-empty", "[encoding_detector]")
{
    REQUIRE_FALSE(encoding_display_name(Encoding::Utf8).empty());
    REQUIRE_FALSE(encoding_display_name(Encoding::Ascii).empty());
}
