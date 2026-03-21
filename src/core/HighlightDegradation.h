/// @file HighlightDegradation.h
/// @brief V20 P07-T04/T05: Large-file degradation and cross-surface highlighting.
///
/// Keeps big files usable while preserving syntax quality. Provides
/// budget-based highlighting, viewport-prioritized tokenization, and
/// cross-surface highlight consistency.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Highlight budget configuration.
struct HighlightBudget
{
    int max_lines_per_pass{500};             ///< Max lines to tokenize per pass
    int viewport_priority_lines{100};        ///< Lines around viewport to prioritize
    int total_line_threshold{10000};          ///< Above this, activate degradation
    double max_time_ms{16.0};                ///< Max time per highlight pass (frame budget)
    bool preserve_keywords{true};            ///< Always preserve keyword tokens
    bool preserve_comments{true};            ///< Always preserve comment tokens
    bool preserve_strings{true};             ///< Always preserve string tokens
};

/// Degradation mode for large files.
enum class DegradationMode
{
    kFull,          ///< Full highlighting (small files)
    kViewport,      ///< Viewport-only highlighting
    kBudgeted,      ///< Budget-based incremental
    kMinimal,       ///< Keywords, strings, comments only
    kNone           ///< No highlighting (emergency fallback)
};

/// Cross-surface highlight target.
enum class HighlightSurface
{
    kEditor,
    kNotebookCell,
    kMarkdownFence,
    kDiffPanel,
    kCanvasEmbed
};

/// Highlight state for a document.
struct HighlightState
{
    DegradationMode mode{DegradationMode::kFull};
    int total_lines{0};
    int highlighted_lines{0};
    int viewport_start{0};
    int viewport_end{0};
    double last_pass_ms{0.0};
    std::string language_id;

    [[nodiscard]] auto coverage_percent() const noexcept -> double
    {
        if (total_lines == 0) return 100.0;
        return (static_cast<double>(highlighted_lines) / static_cast<double>(total_lines)) * 100.0;
    }

    [[nodiscard]] auto is_degraded() const noexcept -> bool
    {
        return mode != DegradationMode::kFull;
    }
};

/// Manages highlight degradation and cross-surface consistency.
class HighlightDegradationService
{
public:
    /// Determine degradation mode for a document.
    [[nodiscard]] auto determine_mode(int line_count) const -> DegradationMode;

    /// Set the highlight budget.
    void set_budget(const HighlightBudget& budget);

    /// Get the current budget.
    [[nodiscard]] auto budget() const -> const HighlightBudget& { return budget_; }

    /// Record a highlight pass result.
    void record_pass(const std::string& document_id, int lines_highlighted, double time_ms);

    /// Get highlight state for a document.
    [[nodiscard]] auto state(const std::string& document_id) const
        -> HighlightState;

    /// Check if a surface should share highlight tokens with the editor.
    [[nodiscard]] auto should_share_tokens(HighlightSurface surface) const -> bool;

    /// List surfaces that support shared highlighting.
    [[nodiscard]] auto shared_surfaces() const -> std::vector<HighlightSurface>;

    /// Total highlight passes recorded.
    [[nodiscard]] auto pass_count() const noexcept -> int { return pass_count_; }

private:
    HighlightBudget budget_;
    std::unordered_map<std::string, HighlightState> states_;
    int pass_count_{0};
};

} // namespace markamp::core
