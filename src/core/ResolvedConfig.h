/// ResolvedConfig.h — Phase 17: Pre-resolved strategy dispatch table
///
/// Eliminates hot-path branches by resolving config options to function pointers
/// at config-change time instead of per-frame. Strategies are small inline
/// functions selected once via rebuild(), then called through pointers.
///
/// Pattern implemented: #11 O(1) typing path guarantee

#pragma once

#include "Config.h"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace markamp::core
{

/// Result of a wrap-strategy call: byte offset where the line should break.
/// Returns 0 if no wrap is needed.
using WrapStrategyFn = std::size_t (*)(std::string_view line, uint32_t wrap_column);
using IndentStrategyFn = std::size_t (*)(std::string_view prev_line);
using WhitespaceRenderMode = uint8_t;
using LineNumberFormatFn = int (*)(int line_number);

// ═══════════════════════════════════════════════════════
// Strategy implementations (static inline, no virtual dispatch)
// ═══════════════════════════════════════════════════════

namespace strategy
{

/// No-op wrap: never wraps.
inline auto no_wrap(std::string_view /*line*/, uint32_t /*wrap_column*/) -> std::size_t
{
    return 0;
}

/// Simple column-based wrap: find last space before wrap_column.
inline auto wrap_at_column(std::string_view line, uint32_t wrap_column) -> std::size_t
{
    if (line.size() <= wrap_column)
    {
        return 0;
    }
    // Scan backward from wrap_column for whitespace
    for (auto pos = static_cast<std::size_t>(wrap_column); pos > 0; --pos)
    {
        if (line[pos] == ' ' || line[pos] == '\t') [[unlikely]]
        {
            return pos;
        }
    }
    // No whitespace found — hard break at column
    return wrap_column;
}

/// No-op indent: zero indent.
inline auto no_indent(std::string_view /*prev_line*/) -> std::size_t
{
    return 0;
}

/// Auto-indent: match leading whitespace of previous line.
inline auto match_indent(std::string_view prev_line) -> std::size_t
{
    std::size_t indent = 0;
    for (char character : prev_line)
    {
        if (character == ' ' || character == '\t')
        {
            ++indent;
        }
        else
        {
            break;
        }
    }
    return indent;
}

/// Line number display: show original number.
inline auto format_line_absolute(int line_number) -> int
{
    return line_number;
}

/// Line number display: show relative to cursor.
inline auto format_line_relative(int line_number) -> int
{
    // Placeholder: actual implementation needs cursor position.
    // The function pointer just enables zero-branch dispatch.
    return line_number;
}

/// Whitespace render modes (used as enum-like constants).
inline constexpr WhitespaceRenderMode kWhitespaceHidden = 0;
inline constexpr WhitespaceRenderMode kWhitespaceVisible = 1;
inline constexpr WhitespaceRenderMode kWhitespaceDimmed = 2;

} // namespace strategy

// ═══════════════════════════════════════════════════════
// ResolvedConfig
// ═══════════════════════════════════════════════════════

/// Pre-resolved dispatch table built from Config::CachedValues.
/// Call rebuild() once on config change, then use the function pointers
/// on every frame/keystroke with zero branching.
class ResolvedConfig
{
public:
    ResolvedConfig() = default;

    /// Resolve all strategy pointers from the given cached config values.
    /// Call this whenever config changes (set() / load()).
    void rebuild(const Config::CachedValues& cached) noexcept
    {
        // Wrap strategy
        if (cached.word_wrap) [[likely]]
        {
            wrap_strategy = strategy::wrap_at_column;
            wrap_column = static_cast<uint32_t>(cached.word_wrap_column);
        }
        else [[unlikely]]
        {
            wrap_strategy = strategy::no_wrap;
            wrap_column = 0;
        }

        // Indent strategy
        if (cached.auto_indent) [[likely]]
        {
            indent_strategy = strategy::match_indent;
        }
        else [[unlikely]]
        {
            indent_strategy = strategy::no_indent;
        }

        // Whitespace rendering
        if (cached.show_whitespace)
        {
            whitespace_mode =
                cached.dim_whitespace ? strategy::kWhitespaceDimmed : strategy::kWhitespaceVisible;
        }
        else
        {
            whitespace_mode = strategy::kWhitespaceHidden;
        }

        // Line number format
        line_number_format = strategy::format_line_absolute;

        // Cache frequently-used scalars
        tab_size = static_cast<uint32_t>(cached.tab_size);
        font_size = static_cast<uint32_t>(cached.font_size);
        show_line_numbers = cached.show_line_numbers;
        show_minimap = cached.show_minimap;
        highlight_current_line = cached.highlight_current_line;
    }

    // ── Strategy function pointers ──

    WrapStrategyFn wrap_strategy{strategy::no_wrap};
    IndentStrategyFn indent_strategy{strategy::no_indent};
    LineNumberFormatFn line_number_format{strategy::format_line_absolute};

    // ── Resolved scalar values ──

    WhitespaceRenderMode whitespace_mode{strategy::kWhitespaceHidden};
    uint32_t wrap_column{80};
    uint32_t tab_size{4};
    uint32_t font_size{14};
    bool show_line_numbers{true};
    bool show_minimap{false};
    bool highlight_current_line{true};
};

} // namespace markamp::core
