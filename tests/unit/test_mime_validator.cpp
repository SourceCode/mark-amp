/// test_mime_validator.cpp — V7 Phase 13: MIME validation tests

#include "core/MimeValidator.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("validate_mime_type: accepts text/plain", "[mime_validator]")
{
    auto result = validate_mime_type("text/plain");
    REQUIRE(result.has_value());
    REQUIRE(*result == "text/plain");
}

TEST_CASE("validate_mime_type: accepts application/json", "[mime_validator]")
{
    auto result = validate_mime_type("application/json");
    REQUIRE(result.has_value());
}

TEST_CASE("validate_mime_type: accepts text/x-yaml", "[mime_validator]")
{
    auto result = validate_mime_type("text/x-yaml");
    REQUIRE(result.has_value());
}

TEST_CASE("validate_mime_type: rejects empty string", "[mime_validator]")
{
    auto result = validate_mime_type("");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidMimeType);
}

TEST_CASE("validate_mime_type: rejects missing slash", "[mime_validator]")
{
    auto result = validate_mime_type("textplain");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidMimeType);
}

TEST_CASE("validate_mime_type: rejects multiple slashes", "[mime_validator]")
{
    auto result = validate_mime_type("text/plain/extra");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidMimeType);
}

TEST_CASE("validate_mime_type: rejects empty type", "[mime_validator]")
{
    auto result = validate_mime_type("/plain");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("validate_mime_type: rejects empty subtype", "[mime_validator]")
{
    auto result = validate_mime_type("text/");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("is_text_mime: returns true for text types", "[mime_validator]")
{
    REQUIRE(is_text_mime("text/plain"));
    REQUIRE(is_text_mime("text/markdown"));
    REQUIRE(is_text_mime("text/html"));
}

TEST_CASE("is_text_mime: returns false for non-text types", "[mime_validator]")
{
    REQUIRE_FALSE(is_text_mime("application/json"));
    REQUIRE_FALSE(is_text_mime("image/png"));
}

TEST_CASE("is_safe_editor_mime: accepts known safe types", "[mime_validator]")
{
    REQUIRE(is_safe_editor_mime("text/markdown"));
    REQUIRE(is_safe_editor_mime("application/json"));
    REQUIRE(is_safe_editor_mime("text/plain"));
}

TEST_CASE("is_safe_editor_mime: rejects unknown types", "[mime_validator]")
{
    REQUIRE_FALSE(is_safe_editor_mime("application/octet-stream"));
    REQUIRE_FALSE(is_safe_editor_mime("image/png"));
}

TEST_CASE("Markdown limits are defined", "[mime_validator]")
{
    REQUIRE(kMaxMarkdownNestingDepth == 32);
    REQUIRE(kMaxMarkdownHeadingDepth == 6);
    REQUIRE(kMaxMarkdownTableColumns == 100);
    REQUIRE(kMaxMarkdownTableRows == 10000);
    REQUIRE(kMaxMarkdownFileSize == 10 * 1024 * 1024);
}
