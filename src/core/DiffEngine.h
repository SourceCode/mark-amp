// ============================================================================
// File: src/core/DiffEngine.h
// Phase 32: Document History — Line-based diff computation (Myers algorithm)
// ============================================================================
#pragma once

#include "HistoryTypes.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Line-based diff engine using the Myers diff algorithm.
/// Computes minimal edit scripts between two text versions.
class DiffEngine
{
public:
    DiffEngine() = default;

    /// Compute the diff between two text strings.
    [[nodiscard]] auto compute_diff(std::string_view old_text,
                                    std::string_view new_text,
                                    int context_lines = 3) const -> DiffResult;

    /// Compute the diff between two sets of lines.
    [[nodiscard]] auto compute_diff_lines(const std::vector<std::string>& old_lines,
                                          const std::vector<std::string>& new_lines,
                                          int context_lines = 3) const -> DiffResult;

    /// Split text into lines.
    [[nodiscard]] static auto split_lines(std::string_view text) -> std::vector<std::string>;

    /// Render a diff as unified diff format text.
    [[nodiscard]] static auto render_unified(const DiffResult& diff) -> std::string;

    /// Render as side-by-side HTML.
    [[nodiscard]] static auto render_side_by_side_html(const DiffResult& diff) -> std::string;

    /// Render as inline HTML.
    [[nodiscard]] static auto render_inline_html(const DiffResult& diff) -> std::string;

private:
    /// Edit operation types for the internal diff algorithm.
    struct EditOp
    {
        enum class Type : uint8_t
        {
            Equal,
            Insert,
            Delete
        };
        Type type{Type::Equal};
        int old_idx{0};
        int new_idx{0};
        int count{1};
    };

    /// Core Myers diff algorithm.
    [[nodiscard]] auto myers_diff(const std::vector<std::string>& old_lines,
                                  const std::vector<std::string>& new_lines) const
        -> std::vector<EditOp>;

    /// Group edit operations into hunks with context.
    [[nodiscard]] auto create_hunks(const std::vector<EditOp>& ops,
                                    const std::vector<std::string>& old_lines,
                                    const std::vector<std::string>& new_lines,
                                    int context_lines) const -> std::vector<DiffHunk>;
};

} // namespace markamp::core
