// test_encoding_detector.cpp — 10 tests for EncodingDetector
#include "core/EncodingDetector.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("detect_encoding identifies UTF-8 BOM", "[encoding]")
{
    std::string bom_utf8 = "\xEF\xBB\xBFHello";
    auto result = detect_encoding(bom_utf8);
    CHECK(result.encoding == Encoding::Utf8Bom);
}

TEST_CASE("detect_encoding identifies UTF-16 LE BOM", "[encoding]")
{
    std::string bom_utf16le = "\xFF\xFEHi";
    auto result = detect_encoding(bom_utf16le);
    CHECK(result.encoding == Encoding::Utf16LE);
}

TEST_CASE("detect_encoding identifies UTF-16 BE BOM", "[encoding]")
{
    std::string bom_utf16be = "\xFE\xFFHi";
    auto result = detect_encoding(bom_utf16be);
    CHECK(result.encoding == Encoding::Utf16BE);
}

TEST_CASE("detect_encoding identifies plain ASCII", "[encoding]")
{
    auto result = detect_encoding("Hello World 123");
    // Pure ASCII can map to Ascii or Utf8
    CHECK((result.encoding == Encoding::Ascii || result.encoding == Encoding::Utf8));
}

TEST_CASE("detect_encoding identifies UTF-8 without BOM", "[encoding]")
{
    // Multibyte UTF-8: é = 0xC3 0xA9
    std::string utf8 = "caf\xC3\xA9";
    auto result = detect_encoding(utf8);
    CHECK(result.encoding == Encoding::Utf8);
}

TEST_CASE("detect_encoding empty input", "[encoding]")
{
    auto result = detect_encoding("");
    // Empty input should not crash; encoding may be Unknown or Ascii
    CHECK((result.encoding == Encoding::Unknown || result.encoding == Encoding::Ascii ||
           result.encoding == Encoding::Utf8));
}

TEST_CASE("strip_bom removes UTF-8 BOM", "[encoding]")
{
    std::string with_bom = "\xEF\xBB\xBFContent";
    auto stripped = strip_bom(with_bom, Encoding::Utf8Bom);
    CHECK(stripped == "Content");
}

TEST_CASE("strip_bom does nothing for non-BOM encoding", "[encoding]")
{
    std::string content = "Hello";
    auto stripped = strip_bom(content, Encoding::Utf8);
    CHECK(stripped == "Hello");
}

TEST_CASE("encoding_display_name returns readable names", "[encoding]")
{
    auto name = encoding_display_name(Encoding::Utf8);
    CHECK_FALSE(name.empty());
    auto bom_name = encoding_display_name(Encoding::Utf8Bom);
    CHECK_FALSE(bom_name.empty());
}

TEST_CASE("encoding_display_name handles all enum values", "[encoding]")
{
    CHECK_FALSE(encoding_display_name(Encoding::Utf8).empty());
    CHECK_FALSE(encoding_display_name(Encoding::Utf8Bom).empty());
    CHECK_FALSE(encoding_display_name(Encoding::Utf16LE).empty());
    CHECK_FALSE(encoding_display_name(Encoding::Utf16BE).empty());
    CHECK_FALSE(encoding_display_name(Encoding::Ascii).empty());
    CHECK_FALSE(encoding_display_name(Encoding::Unknown).empty());
}
