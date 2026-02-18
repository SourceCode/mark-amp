/// test_validation_utils.cpp — V7 Phase 02: Centralized validation utility tests

#include "core/ValidationUtils.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

// ══════════════════════════════════════════════════════════════════════════════
// String Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_string: accepts valid string", "[validation]")
{
    auto result = validate_string("hello", 1, 100);
    REQUIRE(result.has_value());
    REQUIRE(*result == "hello");
}

TEST_CASE("validate_string: rejects too-short string", "[validation]")
{
    auto result = validate_string("", 1, 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::StringTooShort);
}

TEST_CASE("validate_string: rejects too-long string", "[validation]")
{
    std::string long_str(200, 'x');
    auto result = validate_string(long_str, 1, 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::StringTooLong);
}

TEST_CASE("validate_string: accepts exact boundary lengths", "[validation]")
{
    auto result_min = validate_string("a", 1, 10);
    REQUIRE(result_min.has_value());

    auto result_max = validate_string("1234567890", 1, 10);
    REQUIRE(result_max.has_value());
}

// ══════════════════════════════════════════════════════════════════════════════
// Null Byte Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_no_null_bytes: accepts clean string", "[validation]")
{
    auto result = validate_no_null_bytes("hello world");
    REQUIRE(result.has_value());
}

TEST_CASE("validate_no_null_bytes: rejects string with null byte", "[validation]")
{
    std::string with_null = "hello";
    with_null += '\0';
    with_null += "world";
    auto result = validate_no_null_bytes(std::string_view(with_null.data(), with_null.size()));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::NullByte);
}

// ══════════════════════════════════════════════════════════════════════════════
// Display String Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_display_string: accepts normal string", "[validation]")
{
    auto result = validate_display_string("Hello World", 100);
    REQUIRE(result.has_value());
}

TEST_CASE("validate_display_string: allows newlines and tabs", "[validation]")
{
    auto result = validate_display_string("line1\nline2\ttab", 100);
    REQUIRE(result.has_value());
}

TEST_CASE("validate_display_string: rejects empty string", "[validation]")
{
    auto result = validate_display_string("", 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::StringTooShort);
}

TEST_CASE("validate_display_string: rejects control characters", "[validation]")
{
    std::string with_ctrl = "hello";
    with_ctrl += static_cast<char>(1); // SOH control char
    auto result = validate_display_string(with_ctrl, 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidFormat);
}

// ══════════════════════════════════════════════════════════════════════════════
// YAML Key Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_yaml_key: accepts valid key", "[validation]")
{
    auto result = validate_yaml_key("editor.fontSize");
    REQUIRE(result.has_value());
    REQUIRE(*result == "editor.fontSize");
}

TEST_CASE("validate_yaml_key: accepts key with hyphens and underscores", "[validation]")
{
    auto result = validate_yaml_key("my-key_2.value");
    REQUIRE(result.has_value());
}

TEST_CASE("validate_yaml_key: rejects empty key", "[validation]")
{
    auto result = validate_yaml_key("");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidYamlKey);
}

TEST_CASE("validate_yaml_key: rejects key exceeding 128 chars", "[validation]")
{
    std::string long_key(129, 'a');
    auto result = validate_yaml_key(long_key);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidYamlKey);
}

TEST_CASE("validate_yaml_key: rejects spaces", "[validation]")
{
    auto result = validate_yaml_key("my key");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidYamlKey);
}

TEST_CASE("validate_yaml_key: rejects special characters", "[validation]")
{
    auto result = validate_yaml_key("key@value");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidYamlKey);
}

// ══════════════════════════════════════════════════════════════════════════════
// Numeric Range Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_range: accepts value within range", "[validation]")
{
    auto result = validate_range(50, 0, 100);
    REQUIRE(result.has_value());
    REQUIRE(*result == 50);
}

TEST_CASE("validate_range: accepts boundary values", "[validation]")
{
    auto min_result = validate_range(0, 0, 100);
    REQUIRE(min_result.has_value());

    auto max_result = validate_range(100, 0, 100);
    REQUIRE(max_result.has_value());
}

TEST_CASE("validate_range: rejects below minimum", "[validation]")
{
    auto result = validate_range(-1, 0, 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidRange);
}

TEST_CASE("validate_range: rejects above maximum", "[validation]")
{
    auto result = validate_range(101, 0, 100);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidRange);
}

// ══════════════════════════════════════════════════════════════════════════════
// Enum Validation
// ══════════════════════════════════════════════════════════════════════════════

enum class TestEnum : int
{
    A = 0,
    B = 1,
    C = 2,
};

TEST_CASE("validate_enum: accepts valid value", "[validation]")
{
    auto result = validate_enum<TestEnum>(1, 3, "TestEnum");
    REQUIRE(result.has_value());
    REQUIRE(*result == TestEnum::B);
}

TEST_CASE("validate_enum: rejects negative value", "[validation]")
{
    auto result = validate_enum<TestEnum>(-1, 3, "TestEnum");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidEnum);
}

TEST_CASE("validate_enum: rejects out-of-range value", "[validation]")
{
    auto result = validate_enum<TestEnum>(3, 3, "TestEnum");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidEnum);
}

// ══════════════════════════════════════════════════════════════════════════════
// Path Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_path: accepts valid path", "[validation]")
{
    auto result = validate_path("/home/user/documents/file.md");
    REQUIRE(result.has_value());
}

TEST_CASE("validate_path: rejects empty path", "[validation]")
{
    auto result = validate_path("");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidPath);
}

TEST_CASE("validate_path: rejects directory traversal", "[validation]")
{
    auto result = validate_path("/home/user/../../../etc/passwd");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::DirectoryTraversal);
}

TEST_CASE("validate_path: rejects too-long path", "[validation]")
{
    std::string long_path(kMaxPathLength + 1, 'a');
    auto result = validate_path(long_path);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidPath);
}

// ══════════════════════════════════════════════════════════════════════════════
// Filename Validation
// ══════════════════════════════════════════════════════════════════════════════

TEST_CASE("validate_filename: accepts valid filename", "[validation]")
{
    auto result = validate_filename("document.md");
    REQUIRE(result.has_value());
    REQUIRE(*result == "document.md");
}

TEST_CASE("validate_filename: rejects empty filename", "[validation]")
{
    auto result = validate_filename("");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("validate_filename: rejects filename with path separator", "[validation]")
{
    auto result = validate_filename("dir/file.md");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidPath);
}

TEST_CASE("validate_filename: rejects too-long filename", "[validation]")
{
    std::string long_name(kMaxFilenameLength + 1, 'a');
    auto result = validate_filename(long_name);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidPath);
}
