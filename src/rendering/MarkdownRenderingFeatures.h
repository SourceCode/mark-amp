// ============================================================================
// File: src/rendering/MarkdownRenderingFeatures.h
// Phase 8: Markdown Rendering Completeness — Feature types and utilities
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::rendering
{

// ═══════════════════════════════════════════════════════
// Source-Line Mapping
// ═══════════════════════════════════════════════════════

/// Maps a rendered HTML element to its source markdown line.
struct SourceLineMapping
{
    int source_line{0};      ///< 0-indexed markdown source line
    std::string element_id;  ///< HTML element id (auto-generated)
    std::string element_tag; ///< Tag name (p, h1, pre, etc.)
};

// ═══════════════════════════════════════════════════════
// Heading Anchors (for scroll sync)
// ═══════════════════════════════════════════════════════

/// A heading collected during rendering for navigation and sync.
struct HeadingAnchor
{
    int level{0};       ///< Heading level (1–6)
    std::string text;   ///< Plain text content
    std::string slug;   ///< URL-safe slug (used as id)
    int source_line{0}; ///< 0-indexed source line in markdown
};

// ═══════════════════════════════════════════════════════
// Incremental Rendering
// ═══════════════════════════════════════════════════════

/// Describes a dirty region for incremental re-rendering.
struct DirtyRegion
{
    int start_line{0};
    int end_line{0};
    bool full_rerender{false}; ///< True if structural change requires full rerender

    /// Number of lines in the dirty region.
    [[nodiscard]] auto line_count() const noexcept -> int
    {
        return (end_line > start_line) ? (end_line - start_line) : 0;
    }

    /// Whether the region is empty (zero-length).
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return end_line <= start_line && !full_rerender;
    }
};

/// Result of an incremental render operation.
struct IncrementalRenderResult
{
    std::string html_fragment;     ///< Rendered HTML for the changed region
    int replaced_start_line{0};    ///< Start of replaced region in old output
    int replaced_end_line{0};      ///< End of replaced region in old output
    bool was_full_rerender{false}; ///< True if a full re-render was performed

    /// Check if the result represents a successful incremental (partial) update.
    [[nodiscard]] auto is_incremental() const -> bool
    {
        return !was_full_rerender && !html_fragment.empty();
    }
};

// ═══════════════════════════════════════════════════════
// Code Block Controls
// ═══════════════════════════════════════════════════════

/// Configuration for code block rendering enhancements.
struct CodeBlockConfig
{
    bool show_copy_button{true};    ///< Show a copy-to-clipboard button
    bool show_language_label{true}; ///< Show the language name label
    int collapse_threshold{20};     ///< Lines above which code blocks are collapsible
    bool auto_collapse_long{true};  ///< Auto-collapse blocks exceeding threshold

    /// Whether auto-collapse is active (threshold > 0 and enabled).
    [[nodiscard]] auto is_auto_collapse_enabled() const noexcept -> bool
    {
        return auto_collapse_long && collapse_threshold > 0;
    }

    /// Total number of configurable options.
    [[nodiscard]] static constexpr auto option_count() noexcept -> int
    {
        return 4;
    }
};

// ═══════════════════════════════════════════════════════
// Collapsible Sections
// ═══════════════════════════════════════════════════════

/// Detected collapsible section from markdown <details>/<summary>.
struct CollapsibleSection
{
    std::string summary_text; ///< Text within <summary>
    std::string body_html;    ///< Rendered body HTML
    bool default_open{false}; ///< Whether section starts open
    int source_line{0};
};

// ═══════════════════════════════════════════════════════
// Sortable Tables
// ═══════════════════════════════════════════════════════

/// Configuration for sortable table rendering.
struct SortableTableConfig
{
    bool enable_sorting{true};       ///< Add data-sortable attribute
    bool show_sort_indicators{true}; ///< Show ↕ sort indicator in headers
    std::string default_sort_column; ///< Column name to sort by default
};

// ═══════════════════════════════════════════════════════
// Print CSS Configuration
// ═══════════════════════════════════════════════════════

/// Print CSS configuration for preview export.
struct PrintCssConfig
{
    double margin_top_cm{2.0};
    double margin_bottom_cm{2.0};
    double margin_left_cm{2.5};
    double margin_right_cm{2.5};
    bool hide_nav_elements{true};     ///< Hide nav overlay, scroll-to-top, etc.
    bool page_break_before_h1{true};  ///< Insert page break before h1
    bool page_break_before_h2{false}; ///< Insert page break before h2
    bool monochrome_code{false};      ///< Use monochrome code blocks in print
};

// ═══════════════════════════════════════════════════════
// Reading Profile → CSS Mapping
// ═══════════════════════════════════════════════════════

/// Reading profile CSS parameters.
struct ReadingProfileCss
{
    double font_size_px{16.0};
    double line_height{1.6};
    double max_width_px{720.0};
    bool use_serif{false};
    double letter_spacing_px{0.0};
    double paragraph_spacing_px{16.0};
    std::string font_family{"Inter, system-ui, sans-serif"};

    /// Generate the CSS string for this profile.
    [[nodiscard]] auto to_css() const -> std::string
    {
        std::string css;
        css += ".preview-content {\n";
        css += "  font-size: " + std::to_string(font_size_px) + "px;\n";
        css += "  line-height: " + std::to_string(line_height) + ";\n";
        css += "  max-width: " + std::to_string(max_width_px) + "px;\n";
        css += "  margin: 0 auto;\n";
        css += "  font-family: " + font_family + ";\n";
        if (letter_spacing_px > 0.0)
        {
            css += "  letter-spacing: " + std::to_string(letter_spacing_px) + "px;\n";
        }
        css += "}\n";
        css += ".preview-content p {\n";
        css += "  margin-bottom: " + std::to_string(paragraph_spacing_px) + "px;\n";
        css += "}\n";
        return css;
    }
};

// ═══════════════════════════════════════════════════════
// Heading Navigation Overlay
// ═══════════════════════════════════════════════════════

/// Entry in the heading navigation overlay (floating TOC).
struct HeadingNavEntry
{
    int level{0};
    std::string text;
    std::string anchor_id; ///< Slug to scroll to
    bool is_active{false}; ///< Currently visible in viewport
};

/// State of the heading navigation overlay.
struct HeadingNavState
{
    bool visible{false};
    std::vector<HeadingNavEntry> entries;
    int active_index{-1}; ///< Index of the currently active entry

    /// Whether the navigation has any entries.
    [[nodiscard]] auto has_entries() const noexcept -> bool
    {
        return !entries.empty();
    }

    /// Number of navigation entries.
    [[nodiscard]] auto entry_count() const noexcept -> std::size_t
    {
        return entries.size();
    }

    /// Whether an entry is currently active.
    [[nodiscard]] auto has_active() const noexcept -> bool
    {
        return active_index >= 0 && static_cast<std::size_t>(active_index) < entries.size();
    }

    /// Whether the overlay is visible with entries.
    [[nodiscard]] auto is_showing() const noexcept -> bool
    {
        return visible && !entries.empty();
    }
};

} // namespace markamp::rendering
