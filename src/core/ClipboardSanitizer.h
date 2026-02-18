// ============================================================================
// File: src/core/ClipboardSanitizer.h
// Phase 29: Security & Input Validation — Clipboard Content Sanitization
// ============================================================================
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Detected content type of clipboard data.
enum class ClipboardContentType : uint8_t
{
    kPlainText,
    kHtml,
    kMarkdown,
    kUnknown,
};

/// Result of a clipboard sanitization operation.
struct SanitizeResult
{
    std::string content; ///< Sanitized content
    ClipboardContentType content_type{ClipboardContentType::kPlainText};
    size_t original_length{0};         ///< Length before sanitization
    size_t sanitized_length{0};        ///< Length after sanitization
    int32_t modifications{0};          ///< Number of modifications made
    bool was_truncated{false};         ///< Whether content was truncated
    bool had_dangerous_content{false}; ///< Whether dangerous content was found
};

/// Clipboard sanitizer — cleans pasted content before insertion.
///
/// Strips null bytes, normalizes line endings, removes embedded scripts
/// and dangerous HTML tags, and enforces size limits on pasted content.
class ClipboardSanitizer
{
public:
    ClipboardSanitizer();

    /// Sanitize plain text content (null bytes, line endings, size).
    [[nodiscard]] auto sanitize_text(std::string_view text) const -> SanitizeResult;

    /// Sanitize HTML content (strips scripts, iframes, event handlers).
    [[nodiscard]] auto sanitize_html(std::string_view html) const -> SanitizeResult;

    /// Sanitize markdown content (detect and sanitize embedded HTML).
    [[nodiscard]] auto sanitize_markdown(std::string_view markdown) const -> SanitizeResult;

    /// Auto-detect content type and sanitize accordingly.
    [[nodiscard]] auto sanitize_auto(std::string_view content) const -> SanitizeResult;

    /// Detect the content type of clipboard data.
    [[nodiscard]] static auto detect_content_type(std::string_view content) -> ClipboardContentType;

    /// Get the maximum allowed paste size in bytes.
    [[nodiscard]] auto max_paste_size() const noexcept -> size_t;

    /// Set the maximum allowed paste size in bytes.
    auto set_max_paste_size(size_t max_bytes) -> void;

    /// Convert a content type to a human-readable string.
    [[nodiscard]] static auto content_type_name(ClipboardContentType content_type)
        -> std::string_view;

private:
    /// Strip null bytes from content.
    [[nodiscard]] static auto strip_null_bytes(std::string_view input) -> std::string;

    /// Normalize line endings to LF.
    [[nodiscard]] static auto normalize_line_endings(std::string_view input) -> std::string;

    /// Strip dangerous HTML tags (script, style, iframe, object, embed, applet).
    [[nodiscard]] static auto strip_dangerous_tags(std::string_view input) -> std::string;

    /// Strip event handler attributes (onclick, onload, etc.).
    [[nodiscard]] static auto strip_event_handlers(std::string_view input) -> std::string;

    size_t max_paste_size_{1024ULL * 1024ULL}; // 1 MiB default
};

} // namespace markamp::core
