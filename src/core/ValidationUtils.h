/// ValidationUtils.h — V7 Phase 02: Centralized input validation utilities
///
/// All validators return Result<T> from Phase 01. Provides validators for
/// strings, YAML keys, numeric ranges, enums, and paths.

#pragma once

#include "Result.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// String Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Validate that a string is non-empty and within the given length.
[[nodiscard]] auto validate_string(std::string_view input,
                                   size_t min_length,
                                   size_t max_length,
                                   std::string_view field_name = "string") -> Result<std::string>;

/// Validate that a string contains no null bytes.
[[nodiscard]] auto validate_no_null_bytes(std::string_view input,
                                          std::string_view field_name = "string") -> Result<void>;

/// Validate a display string (non-empty, max length, no control chars except newline/tab).
[[nodiscard]] auto validate_display_string(std::string_view input,
                                           size_t max_length,
                                           std::string_view field_name = "display string")
    -> Result<std::string>;

// ══════════════════════════════════════════════════════════════════════════════
// YAML Key Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Maximum allowed YAML key length.
inline constexpr size_t kMaxYamlKeyLength = 128;

/// Validate a YAML key: max 128 chars, [a-zA-Z0-9_.-] only.
[[nodiscard]] auto validate_yaml_key(std::string_view key) -> Result<std::string>;

// ══════════════════════════════════════════════════════════════════════════════
// Numeric Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Validate that an integer is within [min, max].
template <typename T>
[[nodiscard]] auto
validate_range(T value, T min_val, T max_val, std::string_view field_name = "value") -> Result<T>
{
    if (value < min_val || value > max_val)
    {
        return std::unexpected(make_validation_error(
            std::string(field_name) + " = " + std::to_string(value) + " is out of range [" +
                std::to_string(min_val) + ", " + std::to_string(max_val) + "]",
            ErrorCode::InvalidRange));
    }
    return value;
}

// ══════════════════════════════════════════════════════════════════════════════
// Enum Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Validate that an integer value corresponds to a valid enum in [0, max_exclusive).
template <typename EnumT>
[[nodiscard]] auto validate_enum(int value, int max_exclusive, std::string_view enum_name = "enum")
    -> Result<EnumT>
{
    if (value < 0 || value >= max_exclusive)
    {
        return std::unexpected(
            make_validation_error(std::string(enum_name) + " value " + std::to_string(value) +
                                      " is invalid (max " + std::to_string(max_exclusive - 1) + ")",
                                  ErrorCode::InvalidEnum));
    }
    return static_cast<EnumT>(value);
}

// ══════════════════════════════════════════════════════════════════════════════
// Path Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Maximum allowed path length.
inline constexpr size_t kMaxPathLength = 4096;

/// Maximum allowed filename length.
inline constexpr size_t kMaxFilenameLength = 255;

/// Validate a filesystem path: non-empty, no null bytes, within length limit,
/// no directory traversal components.
[[nodiscard]] auto validate_path(std::string_view path_str) -> Result<std::filesystem::path>;

/// Validate a filename: non-empty, no path separators, no null bytes, within length.
[[nodiscard]] auto validate_filename(std::string_view filename) -> Result<std::string>;

} // namespace markamp::core
