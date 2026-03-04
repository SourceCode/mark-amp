#pragma once

/// @file PreserveCaseReplacer.h
/// @brief Phase 34 – Case-preserving replacement logic.

#include <cstdint>
#include <string>

namespace markamp::core
{

/// Detected casing patterns.
enum class CasePattern : std::uint8_t
{
    kAllUpper = 0, ///< "HELLO"
    kAllLower,     ///< "hello"
    kTitleCase,    ///< "Hello"
    kCamelCase,    ///< "helloWorld"
    kPascalCase,   ///< "HelloWorld"
    kMixed         ///< Anything else
};

/// Detect the casing pattern of a string.
[[nodiscard]] auto detect_case_pattern(const std::string& text) -> CasePattern;

/// Transform a replacement string to match the detected casing pattern.
///
/// Examples:
///   - match="HELLO", replacement="world" -> "WORLD" (kAllUpper)
///   - match="hello", replacement="World" -> "world" (kAllLower)
///   - match="Hello", replacement="world" -> "World" (kTitleCase)
///   - match="helloWorld", replacement="foo_bar" -> "fooBar" (kCamelCase)
[[nodiscard]] auto preserve_case_replace(const std::string& matched_text,
                                         const std::string& replacement) -> std::string;

/// Apply a casing pattern to a string.
[[nodiscard]] auto apply_case_pattern(const std::string& text, CasePattern pattern) -> std::string;

} // namespace markamp::core
