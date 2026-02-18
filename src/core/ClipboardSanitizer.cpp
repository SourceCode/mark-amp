// ============================================================================
// File: src/core/ClipboardSanitizer.cpp
// Phase 29: Security & Input Validation — Clipboard Content Sanitization
// ============================================================================

#include "ClipboardSanitizer.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace markamp::core
{

// ── Construction ──

ClipboardSanitizer::ClipboardSanitizer() = default;

// ── Content type detection ──

auto ClipboardSanitizer::detect_content_type(std::string_view content) -> ClipboardContentType
{
    if (content.empty())
    {
        return ClipboardContentType::kPlainText;
    }

    // Check for HTML markers.
    if (content.find('<') != std::string_view::npos)
    {
        // Look for common HTML tags.
        if (content.find("<html") != std::string_view::npos ||
            content.find("<div") != std::string_view::npos ||
            content.find("<p>") != std::string_view::npos ||
            content.find("<span") != std::string_view::npos ||
            content.find("<br") != std::string_view::npos ||
            content.find("<!DOCTYPE") != std::string_view::npos)
        {
            return ClipboardContentType::kHtml;
        }
    }

    // Check for markdown markers.
    if (content.find("# ") != std::string_view::npos ||
        content.find("## ") != std::string_view::npos ||
        content.find("```") != std::string_view::npos ||
        content.find("- [") != std::string_view::npos ||
        content.find("**") != std::string_view::npos ||
        content.find("![") != std::string_view::npos)
    {
        return ClipboardContentType::kMarkdown;
    }

    return ClipboardContentType::kPlainText;
}

auto ClipboardSanitizer::content_type_name(ClipboardContentType content_type) -> std::string_view
{
    switch (content_type)
    {
        case ClipboardContentType::kPlainText:
            return "Plain Text";
        case ClipboardContentType::kHtml:
            return "HTML";
        case ClipboardContentType::kMarkdown:
            return "Markdown";
        case ClipboardContentType::kUnknown:
            return "Unknown";
    }
    return "Unknown";
}

// ── Internal helpers ──

auto ClipboardSanitizer::strip_null_bytes(std::string_view input) -> std::string
{
    std::string result;
    result.reserve(input.size());
    for (const auto kChr : input)
    {
        if (kChr != '\0')
        {
            result += kChr;
        }
    }
    return result;
}

auto ClipboardSanitizer::normalize_line_endings(std::string_view input) -> std::string
{
    std::string result;
    result.reserve(input.size());
    for (size_t idx = 0; idx < input.size(); ++idx)
    {
        if (input[idx] == '\r')
        {
            result += '\n';
            // Skip \n after \r (CRLF → LF).
            if (idx + 1 < input.size() && input[idx + 1] == '\n')
            {
                ++idx;
            }
        }
        else
        {
            result += input[idx];
        }
    }
    return result;
}

auto ClipboardSanitizer::strip_dangerous_tags(std::string_view input) -> std::string
{
    std::string result(input);

    // Tags to strip completely (case-insensitive).
    static const std::vector<std::string> kDangerousTags = {
        "script", "style", "iframe", "object", "embed", "applet", "form"};

    for (const auto& tag : kDangerousTags)
    {
        // Strip opening tags: <script ...> or <script>
        std::string open_pattern = "<" + tag;
        size_t pos = 0;
        while (pos < result.size())
        {
            // Case-insensitive search.
            auto found = std::string::npos;
            for (size_t search_pos = pos; search_pos + open_pattern.size() <= result.size();
                 ++search_pos)
            {
                bool match = true;
                for (size_t chr_idx = 0; chr_idx < open_pattern.size(); ++chr_idx)
                {
                    if (std::tolower(static_cast<unsigned char>(result[search_pos + chr_idx])) !=
                        std::tolower(static_cast<unsigned char>(open_pattern[chr_idx])))
                    {
                        match = false;
                        break;
                    }
                }
                if (match && (search_pos + open_pattern.size() >= result.size() ||
                              result[search_pos + open_pattern.size()] == ' ' ||
                              result[search_pos + open_pattern.size()] == '>' ||
                              result[search_pos + open_pattern.size()] == '/'))
                {
                    found = search_pos;
                    break;
                }
            }

            if (found == std::string::npos)
            {
                break;
            }

            // Find the closing >.
            auto close = result.find('>', found);
            if (close == std::string::npos)
            {
                result.erase(found);
                break;
            }
            result.erase(found, close - found + 1);
            pos = found;
        }

        // Strip closing tags: </script>
        std::string close_pattern = "</" + tag + ">";
        pos = 0;
        while (pos < result.size())
        {
            auto found = std::string::npos;
            for (size_t search_pos = pos; search_pos + close_pattern.size() <= result.size();
                 ++search_pos)
            {
                bool match = true;
                for (size_t chr_idx = 0; chr_idx < close_pattern.size(); ++chr_idx)
                {
                    if (std::tolower(static_cast<unsigned char>(result[search_pos + chr_idx])) !=
                        std::tolower(static_cast<unsigned char>(close_pattern[chr_idx])))
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                {
                    found = search_pos;
                    break;
                }
            }

            if (found == std::string::npos)
            {
                break;
            }
            result.erase(found, close_pattern.size());
            pos = found;
        }
    }

    return result;
}

auto ClipboardSanitizer::strip_event_handlers(std::string_view input) -> std::string
{
    std::string result(input);

    // Remove on* attributes (onclick, onload, onerror, etc.).
    // Pattern: on[a-z]+="..." or on[a-z]+='...'
    size_t pos = 0;
    while (pos < result.size())
    {
        // Look for "on" inside a tag.
        auto on_pos = result.find("on", pos);
        if (on_pos == std::string::npos)
        {
            break;
        }

        // Must be preceded by whitespace (inside a tag).
        if (on_pos > 0 && std::isspace(static_cast<unsigned char>(result[on_pos - 1])) == 0)
        {
            pos = on_pos + 2;
            continue;
        }

        // Check that what follows is lowercase alpha then =.
        size_t end_name = on_pos + 2;
        while (end_name < result.size() &&
               std::islower(static_cast<unsigned char>(result[end_name])) != 0)
        {
            ++end_name;
        }

        if (end_name >= result.size() || result[end_name] != '=')
        {
            pos = on_pos + 2;
            continue;
        }

        // Find the end of the attribute value.
        const size_t val_start = end_name + 1;
        size_t val_end = val_start;

        if (val_start < result.size() && (result[val_start] == '"' || result[val_start] == '\''))
        {
            const char kQuote = result[val_start];
            val_end = result.find(kQuote, val_start + 1);
            if (val_end == std::string::npos)
            {
                val_end = result.size();
            }
            else
            {
                ++val_end; // Include closing quote.
            }
        }

        result.erase(on_pos, val_end - on_pos);
        pos = on_pos;
    }

    return result;
}

// ── Public sanitization API ──

auto ClipboardSanitizer::sanitize_text(std::string_view text) const -> SanitizeResult
{
    SanitizeResult result;
    result.content_type = ClipboardContentType::kPlainText;
    result.original_length = text.size();

    if (text.empty())
    {
        result.content = "";
        result.sanitized_length = 0;
        return result;
    }

    std::string sanitized(text);

    // Strip null bytes.
    auto no_nulls = strip_null_bytes(sanitized);
    if (no_nulls.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(no_nulls);

    // Normalize line endings.
    auto normalized = normalize_line_endings(sanitized);
    if (normalized.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(normalized);

    // Truncate if too large.
    if (sanitized.size() > max_paste_size_)
    {
        sanitized.resize(max_paste_size_);
        result.was_truncated = true;
        ++result.modifications;
    }

    result.content = std::move(sanitized);
    result.sanitized_length = result.content.size();
    return result;
}

auto ClipboardSanitizer::sanitize_html(std::string_view html) const -> SanitizeResult
{
    SanitizeResult result;
    result.content_type = ClipboardContentType::kHtml;
    result.original_length = html.size();

    if (html.empty())
    {
        result.content = "";
        result.sanitized_length = 0;
        return result;
    }

    std::string sanitized(html);

    // Strip null bytes.
    auto no_nulls = strip_null_bytes(sanitized);
    if (no_nulls.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(no_nulls);

    // Strip dangerous tags.
    auto no_danger = strip_dangerous_tags(sanitized);
    if (no_danger.size() != sanitized.size())
    {
        result.had_dangerous_content = true;
        ++result.modifications;
    }
    sanitized = std::move(no_danger);

    // Strip event handlers.
    auto no_handlers = strip_event_handlers(sanitized);
    if (no_handlers.size() != sanitized.size())
    {
        result.had_dangerous_content = true;
        ++result.modifications;
    }
    sanitized = std::move(no_handlers);

    // Normalize line endings.
    auto normalized = normalize_line_endings(sanitized);
    if (normalized.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(normalized);

    // Truncate if too large.
    if (sanitized.size() > max_paste_size_)
    {
        sanitized.resize(max_paste_size_);
        result.was_truncated = true;
        ++result.modifications;
    }

    result.content = std::move(sanitized);
    result.sanitized_length = result.content.size();
    return result;
}

auto ClipboardSanitizer::sanitize_markdown(std::string_view markdown) const -> SanitizeResult
{
    SanitizeResult result;
    result.content_type = ClipboardContentType::kMarkdown;
    result.original_length = markdown.size();

    if (markdown.empty())
    {
        result.content = "";
        result.sanitized_length = 0;
        return result;
    }

    std::string sanitized(markdown);

    // Strip null bytes.
    auto no_nulls = strip_null_bytes(sanitized);
    if (no_nulls.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(no_nulls);

    // Markdown may contain embedded HTML — sanitize it.
    if (sanitized.find('<') != std::string::npos)
    {
        auto no_danger = strip_dangerous_tags(sanitized);
        if (no_danger.size() != sanitized.size())
        {
            result.had_dangerous_content = true;
            ++result.modifications;
        }
        sanitized = std::move(no_danger);

        auto no_handlers = strip_event_handlers(sanitized);
        if (no_handlers.size() != sanitized.size())
        {
            result.had_dangerous_content = true;
            ++result.modifications;
        }
        sanitized = std::move(no_handlers);
    }

    // Normalize line endings.
    auto normalized = normalize_line_endings(sanitized);
    if (normalized.size() != sanitized.size())
    {
        ++result.modifications;
    }
    sanitized = std::move(normalized);

    // Truncate if too large.
    if (sanitized.size() > max_paste_size_)
    {
        sanitized.resize(max_paste_size_);
        result.was_truncated = true;
        ++result.modifications;
    }

    result.content = std::move(sanitized);
    result.sanitized_length = result.content.size();
    return result;
}

auto ClipboardSanitizer::sanitize_auto(std::string_view content) const -> SanitizeResult
{
    const auto kDetected = detect_content_type(content);
    switch (kDetected)
    {
        case ClipboardContentType::kHtml:
            return sanitize_html(content);
        case ClipboardContentType::kMarkdown:
            return sanitize_markdown(content);
        case ClipboardContentType::kPlainText:
        case ClipboardContentType::kUnknown:
        default:
            return sanitize_text(content);
    }
}

// ── Configuration ──

auto ClipboardSanitizer::max_paste_size() const noexcept -> size_t
{
    return max_paste_size_;
}

auto ClipboardSanitizer::set_max_paste_size(size_t max_bytes) -> void
{
    max_paste_size_ = max_bytes;
}

} // namespace markamp::core
