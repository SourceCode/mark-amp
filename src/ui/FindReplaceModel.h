#pragma once

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Search scope for find/replace (Phase 22 Task 3).
enum class FindScope : uint8_t
{
    kDocument,     ///< Current document only
    kSelection,    ///< Current selection
    kAllOpenFiles, ///< Across all open tabs
};

/// Replace action type (Phase 22 Task 2).
enum class ReplaceAction : uint8_t
{
    kReplaceOne,         ///< Replace current match
    kReplaceAll,         ///< Replace all in scope
    kReplaceInSelection, ///< Replace only within selection
};

/// Testable model for Find/Replace in Editor (Phase 22).
///
/// Encapsulates:
/// - Match count and current index tracking
/// - Replace safety (requires confirmation for replace-all)
/// - Scope toggles (document/selection/all open files)
/// - Navigation (next/prev with wrapping)
class FindReplaceModel
{
public:
    /// Set search text.
    void set_find_text(const std::string& text);
    [[nodiscard]] auto find_text() const -> const std::string&;

    /// Set replace text.
    void set_replace_text(const std::string& text);
    [[nodiscard]] auto replace_text() const -> const std::string&;

    // ── Options ─────────────────────────────────────────────────────

    void set_case_sensitive(bool enabled);
    [[nodiscard]] auto case_sensitive() const -> bool;

    void set_regex(bool enabled);
    [[nodiscard]] auto is_regex() const -> bool;

    void set_whole_word(bool enabled);
    [[nodiscard]] auto whole_word() const -> bool;

    // ── Scope ───────────────────────────────────────────────────────

    void set_scope(FindScope scope);
    [[nodiscard]] auto scope() const -> FindScope;

    // ── Match tracking ──────────────────────────────────────────────

    /// Set total match count (from editor engine).
    void set_match_count(int count);
    [[nodiscard]] auto match_count() const -> int;

    /// Current match index (0-based).
    [[nodiscard]] auto current_index() const -> int;

    /// Navigate to next match (wraps around).
    void next_match();

    /// Navigate to previous match (wraps around).
    void prev_match();

    // ── Replace safety ──────────────────────────────────────────────

    /// Format status text: "N of M" or "No results".
    [[nodiscard]] auto status_text() const -> std::string;

    /// Does the given replace action require confirmation?
    [[nodiscard]] static auto needs_confirmation(ReplaceAction action) -> bool;

private:
    std::string find_text_;
    std::string replace_text_;
    bool case_sensitive_{false};
    bool regex_{false};
    bool whole_word_{false};
    FindScope scope_{FindScope::kDocument};
    int match_count_{0};
    int current_index_{0};
};

} // namespace markamp::ui
