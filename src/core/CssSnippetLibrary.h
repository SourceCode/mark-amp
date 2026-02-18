/// @file CssSnippetLibrary.h
/// @brief V9 Phase 38 — Reusable CSS snippet library for vault customization.
#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// CssSnippet — a reusable CSS fragment
// ============================================================================

struct CssSnippet
{
    std::string name;
    std::string description;
    std::string category; ///< e.g. "typography", "colors", "spacing", "animations"
    std::string css_content;
    std::vector<std::string> variables; ///< CSS variable names this snippet defines
    bool is_builtin{false};
    std::chrono::system_clock::time_point created_at;
};

// ============================================================================
// SnippetCategory — well-known snippet categories
// ============================================================================

enum class SnippetCategory : uint8_t
{
    kTypography,
    kColors,
    kSpacing,
    kAnimations,
    kLayout,
    kCustom
};

/// Convert enum to string name.
[[nodiscard]] auto snippet_category_name(SnippetCategory cat) -> std::string;

// ============================================================================
// CssSnippetLibrary
// ============================================================================

/// Manages a library of reusable CSS snippets that can be applied to vault styles.
///
/// Usage:
/// ```cpp
/// CssSnippetLibrary library;
/// library.load_builtins();
/// auto* snippet = library.find_snippet("Dark Headers");
/// if (snippet) {
///     auto merged = library.apply_snippet(*snippet, current_css);
/// }
/// ```
class CssSnippetLibrary
{
public:
    CssSnippetLibrary() = default;

    // ── CRUD ──

    /// Add a snippet to the library.
    void add_snippet(CssSnippet snippet);

    /// Remove a snippet by name. Returns true if found and removed.
    auto remove_snippet(const std::string& name) -> bool;

    /// Find a snippet by name. Returns nullptr if not found.
    [[nodiscard]] auto find_snippet(const std::string& name) const -> const CssSnippet*;

    /// Get all snippets.
    [[nodiscard]] auto all_snippets() const -> const std::vector<CssSnippet>&;

    /// Get snippets filtered by category.
    [[nodiscard]] auto snippets_by_category(const std::string& category) const
        -> std::vector<const CssSnippet*>;

    /// Total snippet count.
    [[nodiscard]] auto snippet_count() const -> std::size_t;

    // ── Application ──

    /// Apply a snippet to existing CSS content. Returns the merged CSS.
    [[nodiscard]] static auto apply_snippet(const CssSnippet& snippet,
                                            const std::string& existing_css) -> std::string;

    /// Generate a preview of CSS with snippet applied.
    [[nodiscard]] static auto generate_preview(const CssSnippet& snippet,
                                               const std::string& base_css) -> std::string;

    // ── Export / Import ──

    /// Export the entire library as JSON.
    [[nodiscard]] auto export_library() const -> std::string;

    /// Import snippets from JSON. Returns number of snippets imported.
    auto import_library(const std::string& json_data) -> int;

    // ── Builtins ──

    /// Load built-in snippet library.
    void load_builtins();

    /// Get all unique category names in the library.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

private:
    std::vector<CssSnippet> snippets_;
};

} // namespace markamp::core
