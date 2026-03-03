#include "TerminalBuffer.h"

#include <algorithm>

namespace markamp::core
{

const TerminalCell TerminalBuffer::kBlankCell = {U' ', AnsiAttributes{}};

TerminalBuffer::TerminalBuffer(int cols, int rows, int max_scrollback)
    : cols_(cols)
    , rows_(rows)
    , max_scrollback_(max_scrollback)
{
    screen_.resize(static_cast<std::size_t>(rows_),
                   std::vector<TerminalCell>(static_cast<std::size_t>(cols_)));
}

void TerminalBuffer::apply(const AnsiOp& op_variant)
{
    std::visit(
        [this](const auto& op)
        {
            using T = std::decay_t<decltype(op)>;

            if constexpr (std::is_same_v<T, TextOutput>)
            {
                current_attrs_ = op.attrs;
                for (const char ch : op.text)
                {
                    if (cursor_col_ >= cols_)
                    {
                        cursor_col_ = 0;
                        cursor_row_++;
                        if (cursor_row_ >= rows_)
                        {
                            scroll_up();
                            cursor_row_ = rows_ - 1;
                        }
                    }
                    screen_[static_cast<std::size_t>(cursor_row_)][static_cast<std::size_t>(
                        cursor_col_)] = {static_cast<char32_t>(ch), current_attrs_};
                    cursor_col_++;
                }
            }
            else if constexpr (std::is_same_v<T, CursorMove>)
            {
                cursor_row_ += op.delta_row;
                cursor_col_ += op.delta_col;
                ensure_cursor_bounds();
            }
            else if constexpr (std::is_same_v<T, CursorPosition>)
            {
                if (op.row >= 0)
                {
                    cursor_row_ = op.row;
                }
                cursor_col_ = op.col;
                ensure_cursor_bounds();
            }
            else if constexpr (std::is_same_v<T, EraseDisplay>)
            {
                if (op.mode == 0)
                {
                    // Erase below (from cursor to end)
                    for (int col = cursor_col_; col < cols_; ++col)
                    {
                        screen_[static_cast<std::size_t>(cursor_row_)]
                               [static_cast<std::size_t>(col)] = kBlankCell;
                    }
                    for (int row = cursor_row_ + 1; row < rows_; ++row)
                    {
                        std::fill(screen_[static_cast<std::size_t>(row)].begin(),
                                  screen_[static_cast<std::size_t>(row)].end(),
                                  kBlankCell);
                    }
                }
                else if (op.mode == 1)
                {
                    // Erase above (from start to cursor)
                    for (int row = 0; row < cursor_row_; ++row)
                    {
                        std::fill(screen_[static_cast<std::size_t>(row)].begin(),
                                  screen_[static_cast<std::size_t>(row)].end(),
                                  kBlankCell);
                    }
                    for (int col = 0; col <= cursor_col_ && col < cols_; ++col)
                    {
                        screen_[static_cast<std::size_t>(cursor_row_)]
                               [static_cast<std::size_t>(col)] = kBlankCell;
                    }
                }
                else if (op.mode == 2)
                {
                    // Erase all
                    for (auto& row : screen_)
                    {
                        std::fill(row.begin(), row.end(), kBlankCell);
                    }
                    cursor_row_ = 0;
                    cursor_col_ = 0;
                }
            }
            else if constexpr (std::is_same_v<T, EraseLine>)
            {
                auto& line = screen_[static_cast<std::size_t>(cursor_row_)];
                if (op.mode == 0)
                {
                    // Erase from cursor to end of line
                    for (int col = cursor_col_; col < cols_; ++col)
                    {
                        line[static_cast<std::size_t>(col)] = kBlankCell;
                    }
                }
                else if (op.mode == 1)
                {
                    // Erase from start to cursor
                    for (int col = 0; col <= cursor_col_ && col < cols_; ++col)
                    {
                        line[static_cast<std::size_t>(col)] = kBlankCell;
                    }
                }
                else if (op.mode == 2)
                {
                    // Erase entire line
                    std::fill(line.begin(), line.end(), kBlankCell);
                }
            }
            else if constexpr (std::is_same_v<T, LineFeed>)
            {
                cursor_row_++;
                if (cursor_row_ >= rows_)
                {
                    scroll_up();
                    cursor_row_ = rows_ - 1;
                }
            }
            else if constexpr (std::is_same_v<T, CarriageReturn>)
            {
                cursor_col_ = 0;
            }
            // SetTitle, SetCwd, Bell are handled at the service level, not buffer
        },
        op_variant);
}

void TerminalBuffer::apply_batch(const std::vector<AnsiOp>& ops)
{
    for (const auto& op_variant : ops)
    {
        apply(op_variant);
    }
}

void TerminalBuffer::resize(int new_cols, int new_rows)
{
    if (new_cols == cols_ && new_rows == rows_)
    {
        return;
    }

    cols_ = std::max(1, new_cols);
    rows_ = std::max(1, new_rows);

    screen_.resize(static_cast<std::size_t>(rows_));
    for (auto& row : screen_)
    {
        row.resize(static_cast<std::size_t>(cols_), kBlankCell);
    }

    ensure_cursor_bounds();
}

auto TerminalBuffer::cols() const -> int
{
    return cols_;
}
auto TerminalBuffer::rows() const -> int
{
    return rows_;
}
auto TerminalBuffer::cursor_row() const -> int
{
    return cursor_row_;
}
auto TerminalBuffer::cursor_col() const -> int
{
    return cursor_col_;
}

auto TerminalBuffer::cell_at(int row, int col) const -> const TerminalCell&
{
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_)
    {
        return kBlankCell;
    }
    return screen_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
}

auto TerminalBuffer::get_line_text(int row) const -> std::string
{
    if (row < 0 || row >= rows_)
    {
        return {};
    }

    std::string result;
    result.reserve(static_cast<std::size_t>(cols_));
    for (const auto& cell : screen_[static_cast<std::size_t>(row)])
    {
        if (cell.character < 128)
        {
            result += static_cast<char>(cell.character);
        }
        else
        {
            result += '?';
        }
    }

    // Trim trailing spaces
    while (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }
    return result;
}

auto TerminalBuffer::scrollback_lines() const -> int
{
    return static_cast<int>(scrollback_.size());
}

auto TerminalBuffer::get_scrollback_line(int index) const -> std::string
{
    if (index < 0 || index >= static_cast<int>(scrollback_.size()))
    {
        return {};
    }

    std::string result;
    for (const auto& cell : scrollback_[static_cast<std::size_t>(index)])
    {
        if (cell.character < 128)
        {
            result += static_cast<char>(cell.character);
        }
        else
        {
            result += '?';
        }
    }
    while (!result.empty() && result.back() == ' ')
    {
        result.pop_back();
    }
    return result;
}

void TerminalBuffer::clear_screen()
{
    for (auto& row : screen_)
    {
        std::fill(row.begin(), row.end(), kBlankCell);
    }
    cursor_row_ = 0;
    cursor_col_ = 0;
}

void TerminalBuffer::clear_scrollback()
{
    scrollback_.clear();
}

auto TerminalBuffer::get_selection_text(int start_row,
                                        int start_col,
                                        int end_row,
                                        int end_col) const -> std::string
{
    if (start_row > end_row || (start_row == end_row && start_col > end_col))
    {
        return {};
    }

    std::string result;
    for (int row = start_row; row <= end_row && row < rows_; ++row)
    {
        const int col_start = (row == start_row) ? start_col : 0;
        const int col_end = (row == end_row) ? end_col : cols_ - 1;

        for (int col = col_start; col <= col_end && col < cols_; ++col)
        {
            const auto& cell =
                screen_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
            if (cell.character < 128)
            {
                result += static_cast<char>(cell.character);
            }
        }
        if (row < end_row)
        {
            result += '\n';
        }
    }
    return result;
}

void TerminalBuffer::scroll_up()
{
    // Move top row to scrollback
    scrollback_.push_back(std::move(screen_[0]));

    // Enforce scrollback limit
    if (static_cast<int>(scrollback_.size()) > max_scrollback_)
    {
        scrollback_.erase(scrollback_.begin());
    }

    // Shift screen rows up
    for (int row = 0; row < rows_ - 1; ++row)
    {
        screen_[static_cast<std::size_t>(row)] =
            std::move(screen_[static_cast<std::size_t>(row + 1)]);
    }

    // Clear bottom row
    screen_[static_cast<std::size_t>(rows_ - 1)] =
        std::vector<TerminalCell>(static_cast<std::size_t>(cols_), kBlankCell);
}

void TerminalBuffer::ensure_cursor_bounds()
{
    cursor_row_ = std::clamp(cursor_row_, 0, rows_ - 1);
    cursor_col_ = std::clamp(cursor_col_, 0, cols_ - 1);
}

} // namespace markamp::core
