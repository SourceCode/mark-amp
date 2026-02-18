#pragma once

/// @file SearchAccessibility.h
/// @brief V9 Phase 10 – Accessibility metadata for search components.
///
/// Header-only implementation of search-related accessibility support:
///   - Screen reader announcements for result counts
///   - Navigation status announcements
///   - ARIA-like role/label/description metadata
///   - Live region support for dynamic result updates

#include "Search.h"

#include <string>

namespace markamp::core
{

// ────────────────────────────────────────────────────────────
// SearchAccessibilityInfo
// ────────────────────────────────────────────────────────────

/// Accessibility metadata for a search UI element.
struct SearchAccessibilityInfo
{
    /// ARIA-like role (e.g., "searchbox", "listbox", "option").
    std::string role;

    /// Accessible label (e.g., "Search input", "Result 3 of 42").
    std::string label;

    /// Extended description for verbose mode.
    std::string description;

    /// Live region priority: "polite", "assertive", or empty for none.
    std::string live_region;
};

// ────────────────────────────────────────────────────────────
// SearchAccessibility
// ────────────────────────────────────────────────────────────

/// Generates accessibility metadata for search interactions.
class SearchAccessibility
{
public:
    SearchAccessibility() = default;

    /// Announce the result count for screen readers.
    [[nodiscard]] static auto announce_result_count(int count) -> std::string
    {
        if (count == 0)
        {
            return "No results found";
        }
        if (count == 1)
        {
            return "1 result found";
        }
        return std::to_string(count) + " results found";
    }

    /// Announce navigation position for screen readers.
    [[nodiscard]] static auto announce_navigation(int current, int total) -> std::string
    {
        if (total == 0)
        {
            return "No results to navigate";
        }
        return "Result " + std::to_string(current + 1) + " of " + std::to_string(total);
    }

    /// Announce a replace operation.
    [[nodiscard]] static auto announce_replace(int replaced, int total_matches) -> std::string
    {
        if (replaced == 0)
        {
            return "No replacements made";
        }
        return std::to_string(replaced) + " of " + std::to_string(total_matches) +
               " occurrences replaced";
    }

    /// Get accessibility info for the search input field.
    [[nodiscard]] static auto get_search_field_info() -> SearchAccessibilityInfo
    {
        return {"searchbox",
                "Search",
                "Type to search across all documents. "
                "Press Enter to search, Escape to close.",
                ""};
    }

    /// Get accessibility info for the replace input field.
    [[nodiscard]] static auto get_replace_field_info() -> SearchAccessibilityInfo
    {
        return {"textbox",
                "Replace",
                "Type replacement text. "
                "Press Enter to replace current match.",
                ""};
    }

    /// Get accessibility info for a search result item.
    [[nodiscard]] static auto get_result_item_info(const SearchHit& hit, int index, int total)
        -> SearchAccessibilityInfo
    {
        std::string label = "Result " + std::to_string(index + 1) + " of " + std::to_string(total) +
                            ": " + hit.doc_title;

        std::string desc;
        if (!hit.snippet.empty())
        {
            desc = "Snippet: " + hit.snippet;
        }

        return {"option", label, desc, ""};
    }

    /// Get accessibility info for the results list.
    [[nodiscard]] static auto get_results_list_info(int count) -> SearchAccessibilityInfo
    {
        return {
            "listbox",
            "Search Results",
            announce_result_count(count),
            "polite" // Updates are announced politely
        };
    }

    /// Get accessibility info for a search mode toggle.
    [[nodiscard]] static auto get_toggle_info(const std::string& toggle_name, bool is_active)
        -> SearchAccessibilityInfo
    {
        return {"switch",
                toggle_name + (is_active ? " (active)" : " (inactive)"),
                "Press to toggle " + toggle_name,
                ""};
    }

    /// Announce saved search action.
    [[nodiscard]] static auto announce_saved_search(const std::string& action,
                                                    const std::string& search_name) -> std::string
    {
        return "Search \"" + search_name + "\" " + action;
    }

    /// Announce export completion.
    [[nodiscard]] static auto announce_export(const std::string& format, int result_count)
        -> std::string
    {
        return std::to_string(result_count) + " results exported as " + format;
    }
};

} // namespace markamp::core
