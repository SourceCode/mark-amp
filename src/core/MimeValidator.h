/// MimeValidator.h — V7 Phase 13: MIME validation and markdown parsing limits
///
/// Validates MIME types and sets limits for Markdown parsing depth/complexity.

#pragma once

#include "Result.h"

#include <string>
#include <string_view>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Markdown Parsing Limits
// ══════════════════════════════════════════════════════════════════════════════

inline constexpr int kMaxMarkdownNestingDepth = 32;
inline constexpr int kMaxMarkdownHeadingDepth = 6;
inline constexpr int kMaxMarkdownTableColumns = 100;
inline constexpr int kMaxMarkdownTableRows = 10000;
inline constexpr size_t kMaxMarkdownFileSize = static_cast<size_t>(10) * 1024 * 1024; // 10 MiB

// ══════════════════════════════════════════════════════════════════════════════
// MIME Validation
// ══════════════════════════════════════════════════════════════════════════════

/// Validate a MIME type string (e.g., "text/plain", "application/json").
[[nodiscard]] auto validate_mime_type(std::string_view mime) -> Result<std::string>;

/// Check if a MIME type is a text type.
[[nodiscard]] auto is_text_mime(std::string_view mime) -> bool;

/// Check if a MIME type is a known safe type for the editor.
[[nodiscard]] auto is_safe_editor_mime(std::string_view mime) -> bool;

} // namespace markamp::core
