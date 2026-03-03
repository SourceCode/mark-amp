#pragma once

#include "AnsiParser.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A single cell in the terminal character grid.
struct TerminalCell
{
    char32_t character{U' '};
    AnsiAttributes attributes;
};

/// Phase 21 Task 3: Terminal character buffer with scrollback.
/// Maintains a visible screen grid (rows × cols) and a scrollback ring buffer.
/// Processes AnsiOp commands and maintains cursor position.
class TerminalBuffer
{
public:
    TerminalBuffer(int cols = 80, int rows = 24, int max_scrollback = 10000);

    /// Apply a single AnsiOp to the buffer.
    void apply(const AnsiOp& op_variant);

    /// Apply a batch of operations.
    void apply_batch(const std::vector<AnsiOp>& ops);

    /// Resize the terminal grid.
    void resize(int new_cols, int new_rows);

    [[nodiscard]] auto cols() const -> int;
    [[nodiscard]] auto rows() const -> int;
    [[nodiscard]] auto cursor_row() const -> int;
    [[nodiscard]] auto cursor_col() const -> int;

    /// Get a cell at the given screen position.
    [[nodiscard]] auto cell_at(int row, int col) const -> const TerminalCell&;

    /// Get the text content of a screen line.
    [[nodiscard]] auto get_line_text(int row) const -> std::string;

    /// Number of lines in scrollback.
    [[nodiscard]] auto scrollback_lines() const -> int;

    /// Get text of a scrollback line (0 = oldest).
    [[nodiscard]] auto get_scrollback_line(int index) const -> std::string;

    /// Clear the visible screen.
    void clear_screen();

    /// Clear scrollback only.
    void clear_scrollback();

    /// Extract selected text (inclusive ranges).
    [[nodiscard]] auto
    get_selection_text(int start_row, int start_col, int end_row, int end_col) const -> std::string;

private:
    int cols_;
    int rows_;
    int max_scrollback_;
    int cursor_row_{0};
    int cursor_col_{0};
    AnsiAttributes current_attrs_;

    std::vector<std::vector<TerminalCell>> screen_;
    std::vector<std::vector<TerminalCell>> scrollback_;

    void scroll_up();
    void ensure_cursor_bounds();

    static const TerminalCell kBlankCell;
};

} // namespace markamp::core
