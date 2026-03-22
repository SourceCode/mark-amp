/// @file EditorFeatureCompletionAuditor.h
/// @brief V23 Phase 05 — Editor, preview, search, navigation, and diagnostics completion auditor.
///
/// Audits that editor-side features have real implementations instead of empty
/// backends, placeholder renderers, or incomplete diagnostic/navigation chains.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// EditorFeatureArea — categories of editor-side features
// ============================================================================

enum class EditorFeatureArea : uint8_t
{
    kSearch,
    kPreview,
    kNavigation,
    kDiagnostics,
    kCodeLens,
    kPeek,
    kSymbolLookup,
    kQuickFix,
};

/// Label for EditorFeatureArea.
[[nodiscard]] constexpr auto editor_feature_area_label(EditorFeatureArea area) -> const char*
{
    switch (area)
    {
    case EditorFeatureArea::kSearch:       return "Search";
    case EditorFeatureArea::kPreview:      return "Preview";
    case EditorFeatureArea::kNavigation:   return "Navigation";
    case EditorFeatureArea::kDiagnostics:  return "Diagnostics";
    case EditorFeatureArea::kCodeLens:     return "CodeLens";
    case EditorFeatureArea::kPeek:         return "Peek";
    case EditorFeatureArea::kSymbolLookup: return "SymbolLookup";
    case EditorFeatureArea::kQuickFix:     return "QuickFix";
    }
    return "Unknown";
}

// ============================================================================
// EditorFeatureCoverageItem — evidence that a feature is implemented
// ============================================================================

struct EditorFeatureCoverageItem
{
    EditorFeatureArea area{EditorFeatureArea::kSearch};
    std::string feature_name;
    bool is_implemented{false};     ///< Not returning empty/placeholder
    bool is_indexed{false};         ///< For search: backed by real index
    bool has_error_handling{false};
    bool has_async_support{false};  ///< Long-running ops are async
    std::string evidence_file;
    int evidence_line{0};
    std::string notes;

    /// Whether this feature is fully covered.
    [[nodiscard]] auto is_complete() const noexcept -> bool
    {
        return is_implemented && has_error_handling;
    }
};

// ============================================================================
// EditorFeatureGapReport — summary of editor feature coverage
// ============================================================================

struct EditorFeatureGapReport
{
    std::size_t total_features{0};
    std::size_t implemented{0};
    std::size_t placeholder{0};
    std::size_t search_gaps{0};
    std::size_t preview_gaps{0};
    std::size_t navigation_gaps{0};
    std::size_t diagnostics_gaps{0};
    std::size_t code_lens_gaps{0};
    std::size_t peek_gaps{0};
    std::size_t symbol_lookup_gaps{0};
    std::size_t quick_fix_gaps{0};

    [[nodiscard]] auto has_gaps() const noexcept -> bool { return placeholder > 0; }

    [[nodiscard]] auto coverage_pct() const noexcept -> double
    {
        return total_features > 0
            ? (static_cast<double>(implemented) / static_cast<double>(total_features)) * 100.0
            : 100.0;
    }
};

// ============================================================================
// EditorFeatureCompletionAuditor — the auditing engine
// ============================================================================

class EditorFeatureCompletionAuditor
{
public:
    EditorFeatureCompletionAuditor() = default;

    // ── Registration ──

    void add_item(EditorFeatureCoverageItem item);
    void add_items(std::vector<EditorFeatureCoverageItem> items);

    // ── Queries ──

    [[nodiscard]] auto item_count() const noexcept -> std::size_t;
    [[nodiscard]] auto items_by_area(EditorFeatureArea area) const
        -> std::vector<const EditorFeatureCoverageItem*>;
    [[nodiscard]] auto complete_items() const
        -> std::vector<const EditorFeatureCoverageItem*>;
    [[nodiscard]] auto incomplete_items() const
        -> std::vector<const EditorFeatureCoverageItem*>;
    [[nodiscard]] auto placeholder_items() const
        -> std::vector<const EditorFeatureCoverageItem*>;

    // ── Report ──

    [[nodiscard]] auto gap_report() const -> EditorFeatureGapReport;

    // ── Clear ──

    void clear();

    // ── Export ──

    [[nodiscard]] auto export_json() const -> std::string;
    [[nodiscard]] auto export_markdown() const -> std::string;

private:
    std::vector<EditorFeatureCoverageItem> items_;
};

} // namespace markamp::core
