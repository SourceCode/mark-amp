// ============================================================================
// File: src/rendering/CalloutBlockRenderer.h
// Phase 29: Advanced Block Types — Callout/alert block renderer
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace markamp::rendering
{

/// Type of callout/alert block (GitHub-style).
enum class CalloutType : uint8_t
{
    kNote,
    kTip,
    kImportant,
    kWarning,
    kCaution
};

/// Rendered callout with styling information.
struct CalloutRenderResult
{
    std::string html;
    CalloutType type{CalloutType::kNote};
    std::string icon;           // SVG icon or Unicode glyph
    std::string css_class;      // CSS class for theming
    bool is_collapsible{false}; // Whether rendered as <details>/<summary>
};

/// Detects and renders GitHub-style callout/alert blocks.
/// Syntax: > [!NOTE], > [!TIP], > [!IMPORTANT], > [!WARNING], > [!CAUTION]
class CalloutBlockRenderer
{
public:
    CalloutBlockRenderer();

    /// Detect if a blockquote contains a callout marker on the first line.
    [[nodiscard]] auto detect_callout(std::string_view blockquote_content) const -> bool;

    /// Parse the callout type from the first line of a blockquote.
    [[nodiscard]] auto parse_callout_type(std::string_view first_line) const -> CalloutType;

    /// Render a callout block to HTML with appropriate styling.
    /// When is_collapsible is true, wraps in <details>/<summary> instead of <div>.
    [[nodiscard]] auto render(std::string_view content,
                              CalloutType type,
                              bool is_collapsible = false,
                              bool is_collapsed = true) const -> CalloutRenderResult;

    /// Get the display name for a callout type.
    [[nodiscard]] static auto type_name(CalloutType type) -> std::string_view;

    /// Get the icon for a callout type.
    [[nodiscard]] static auto type_icon(CalloutType type) -> std::string_view;

    /// Get the CSS color class for a callout type.
    [[nodiscard]] static auto type_css_class(CalloutType type) -> std::string_view;

    /// Total number of CalloutType values.
    [[nodiscard]] static constexpr auto type_count() noexcept -> int
    {
        return 5;
    }

    /// Number of registered callout type strings in the lookup map.
    [[nodiscard]] auto supported_type_count() const -> int
    {
        return static_cast<int>(type_map_.size());
    }

private:
    std::unordered_map<std::string, CalloutType> type_map_;
};

} // namespace markamp::rendering
