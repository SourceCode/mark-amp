/// @file KanbanEngine.cpp
/// @brief V4 Phase 25 – Kanban Board View (pure-logic engine) implementation.

#include "ui/KanbanEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <regex>
#include <sstream>

namespace markamp::ui
{

// ============================================================================
// KanbanBoard helpers
// ============================================================================

auto KanbanBoard::total_cards() const -> int
{
    int total = 0;
    for (const auto& column : columns)
    {
        total += static_cast<int>(column.cards.size());
    }
    return total;
}

auto KanbanBoard::find_card(const std::string& task_id) const -> std::optional<std::pair<int, int>>
{
    for (int col_idx = 0; col_idx < static_cast<int>(columns.size()); ++col_idx)
    {
        const auto& column = columns[static_cast<size_t>(col_idx)];
        for (int card_idx = 0; card_idx < static_cast<int>(column.cards.size()); ++card_idx)
        {
            if (column.cards[static_cast<size_t>(card_idx)].id == task_id)
            {
                return std::pair{col_idx, card_idx};
            }
        }
    }
    return std::nullopt;
}

// ============================================================================
// Constructor
// ============================================================================

KanbanEngine::KanbanEngine(markamp::core::EventBus& event_bus,
                           markamp::core::VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Parse kanban markdown
// ============================================================================

auto KanbanEngine::parse_kanban_markdown(const std::string& markdown) -> KanbanBoard
{
    KanbanBoard board;

    std::istringstream stream(markdown);
    std::string line;
    int line_number = 0;
    int current_col = -1;

    // First line as title if it's H1
    bool found_title = false;

    while (std::getline(stream, line))
    {
        ++line_number;

        // Detect H1 title
        if (!found_title && line.size() >= 2 && line.substr(0, 2) == "# ")
        {
            board.title = line.substr(2);
            found_title = true;
            continue;
        }

        // Detect H2 column heading
        if (line.size() >= 3 && line.substr(0, 3) == "## ")
        {
            KanbanColumn column;
            column.heading_text = line;
            column.title = line.substr(3);
            column.line_number = line_number;

            // Check for WIP limit: ## Column [3]
            auto bracket_open = column.title.rfind('[');
            auto bracket_close = column.title.rfind(']');
            if (bracket_open != std::string::npos && bracket_close != std::string::npos &&
                bracket_close > bracket_open + 1 && bracket_close == column.title.size() - 1)
            {
                auto num_str =
                    column.title.substr(bracket_open + 1, bracket_close - bracket_open - 1);
                // Verify all digits
                bool all_digits = !num_str.empty();
                for (const char character : num_str)
                {
                    if (character < '0' || character > '9')
                    {
                        all_digits = false;
                        break;
                    }
                }
                if (all_digits)
                {
                    column.max_cards = std::stoi(num_str);
                    // Strip WIP limit from title
                    auto title_end = bracket_open;
                    while (title_end > 0 && column.title[title_end - 1] == ' ')
                    {
                        --title_end;
                    }
                    column.title = column.title.substr(0, title_end);
                }
            }

            board.columns.push_back(std::move(column));
            current_col = static_cast<int>(board.columns.size()) - 1;
            continue;
        }

        // Parse task lines within current column
        if (current_col >= 0)
        {
            auto task = markamp::core::TaskService::parse_task_line(line, line_number);
            if (task.has_value())
            {
                task->id =
                    "col" + std::to_string(current_col) + "_card" +
                    std::to_string(board.columns[static_cast<size_t>(current_col)].cards.size());
                board.columns[static_cast<size_t>(current_col)].cards.push_back(std::move(*task));
            }
        }
    }

    return board;
}

// ============================================================================
// Move card between columns
// ============================================================================

auto KanbanEngine::move_card(
    const std::string& markdown, int from_col, int card_idx, int to_col, int to_row) -> std::string
{
    // Parse the board to find line numbers
    auto board = parse_kanban_markdown(markdown);

    if (from_col < 0 || from_col >= static_cast<int>(board.columns.size()) || to_col < 0 ||
        to_col >= static_cast<int>(board.columns.size()) || card_idx < 0 ||
        card_idx >= static_cast<int>(board.columns[static_cast<size_t>(from_col)].cards.size()))
    {
        return markdown; // Invalid indices
    }

    // Get the task line to move
    const auto& card =
        board.columns[static_cast<size_t>(from_col)].cards[static_cast<size_t>(card_idx)];
    const int source_line = card.line_number;

    // Split markdown into lines
    std::vector<std::string> lines;
    std::istringstream stream(markdown);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }

    if (source_line < 1 || source_line > static_cast<int>(lines.size()))
    {
        return markdown;
    }

    // Extract the line to move
    const std::string moved_line = lines[static_cast<size_t>(source_line - 1)];
    lines.erase(lines.begin() + source_line - 1);

    // Re-parse to find insertion point after removal
    std::string modified;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
        {
            modified += "\n";
        }
        modified += lines[idx];
    }

    auto new_board = parse_kanban_markdown(modified);

    // Find insertion point in target column
    int insert_line = -1;
    if (to_col < static_cast<int>(new_board.columns.size()))
    {
        const auto& target_col = new_board.columns[static_cast<size_t>(to_col)];
        if (to_row >= 0 && to_row < static_cast<int>(target_col.cards.size()))
        {
            insert_line = target_col.cards[static_cast<size_t>(to_row)].line_number;
        }
        else if (!target_col.cards.empty())
        {
            // Insert after last card in target column
            insert_line = target_col.cards.back().line_number + 1;
        }
        else
        {
            // Empty column: insert after column heading
            insert_line = target_col.line_number + 1;
        }
    }

    if (insert_line < 1)
    {
        return markdown;
    }

    // Re-split modified into lines
    lines.clear();
    std::istringstream stream2(modified);
    while (std::getline(stream2, line))
    {
        lines.push_back(line);
    }

    // Insert at target position
    auto insert_idx = static_cast<size_t>(insert_line - 1);
    if (insert_idx > lines.size())
    {
        insert_idx = lines.size();
    }
    lines.insert(lines.begin() + static_cast<int>(insert_idx), moved_line);

    // Rebuild markdown
    std::string result;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
        {
            result += "\n";
        }
        result += lines[idx];
    }

    return result;
}

// ============================================================================
// Reorder card within column
// ============================================================================

auto KanbanEngine::reorder_card(const std::string& markdown, int col_idx, int from_row, int to_row)
    -> std::string
{
    return move_card(markdown, col_idx, from_row, col_idx, to_row);
}

// ============================================================================
// Toggle card checkbox
// ============================================================================

auto KanbanEngine::toggle_card(const std::string& markdown, int col_idx, int card_idx)
    -> std::string
{
    auto board = parse_kanban_markdown(markdown);

    if (col_idx < 0 || col_idx >= static_cast<int>(board.columns.size()) || card_idx < 0 ||
        card_idx >= static_cast<int>(board.columns[static_cast<size_t>(col_idx)].cards.size()))
    {
        return markdown;
    }

    const auto& card =
        board.columns[static_cast<size_t>(col_idx)].cards[static_cast<size_t>(card_idx)];
    const int target_line = card.line_number;

    std::vector<std::string> lines;
    std::istringstream stream(markdown);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }

    if (target_line < 1 || target_line > static_cast<int>(lines.size()))
    {
        return markdown;
    }

    std::string& target = lines[static_cast<size_t>(target_line - 1)];

    // Toggle [ ] <-> [x]
    auto checkbox_pos = target.find("- [ ]");
    if (checkbox_pos != std::string::npos)
    {
        target.replace(checkbox_pos, 5, "- [x]");
    }
    else
    {
        auto done_pos = target.find("- [x]");
        if (done_pos != std::string::npos)
        {
            target.replace(done_pos, 5, "- [ ]");
        }
    }

    std::string result;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
        {
            result += "\n";
        }
        result += lines[idx];
    }

    return result;
}

} // namespace markamp::ui
