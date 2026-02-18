/// StringGuards.h — V7 Phase 07: String length guards
///
/// Prevents memory exhaustion and display corruption via string length limits.
/// UTF-8 aware truncation and display string validation.

#pragma once

#include "Result.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Constants
// ══════════════════════════════════════════════════════════════════════════════

inline constexpr size_t kMaxFilenameGuard = 255;
inline constexpr size_t kMaxPathGuard = 4096;
inline constexpr size_t kMaxYamlValueGuard = 65536; // 64 KiB
inline constexpr size_t kMaxSearchQueryGuard = 1024;
inline constexpr size_t kMaxTitleGuard = 512;
inline constexpr size_t kMaxToastMessageGuard = 256;
inline constexpr size_t kMaxLogMessageGuard = 4096;
inline constexpr size_t kMaxConfigValueGuard = 16384; // 16 KiB
inline constexpr size_t kMaxClipboardGuard = 1048576; // 1 MiB

// ══════════════════════════════════════════════════════════════════════════════
// Truncation
// ══════════════════════════════════════════════════════════════════════════════

/// Truncate a string to max_len bytes, respecting UTF-8 boundaries.
/// Appends "..." if truncated.
[[nodiscard]] auto truncate_safe(std::string_view input, size_t max_len) -> std::string;

/// Apply a length guard: validate and truncate if needed.
/// Returns the (potentially truncated) string if within limits.
[[nodiscard]] auto apply_length_guard(std::string_view input,
                                      size_t max_len,
                                      std::string_view field_name = "string") -> std::string;

} // namespace markamp::core
