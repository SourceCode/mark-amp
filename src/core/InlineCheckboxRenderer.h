#pragma once

/// @file InlineCheckboxRenderer.h
/// @brief V9 Phase 45 Task 7 – Checkbox toggling logic for task lists.
///
/// Header-only, pure logic. Toggles `- [ ]` ↔ `- [x]` and provides
/// task statistics for documents.

#include <cstdint>
#include <sstream>
#include <string>

namespace markamp::core
{

/// Task list statistics for a document.
struct TaskStats
{
    int total{0};     ///< Total number of task items
    int completed{0}; ///< Number of checked items

    /// Completion percentage (0.0–1.0).
    [[nodiscard]] auto completion_ratio() const noexcept -> double
    {
        if (total == 0)
        {
            return 0.0;
        }
        return static_cast<double>(completed) / static_cast<double>(total);
    }

    /// Completion percentage (0–100).
    [[nodiscard]] auto completion_percent() const noexcept -> int
    {
        if (total == 0)
        {
            return 0;
        }
        return static_cast<int>(completion_ratio() * 100.0);
    }
};

/// Provides checkbox toggling logic for markdown task lists.
/// Handles `- [ ]`, `- [x]`, `* [ ]`, `* [x]`, `+ [ ]`, `+ [x]` patterns.
class InlineCheckboxRenderer
{
public:
    InlineCheckboxRenderer() = default;

    /// Check if a line is a task list item.
    [[nodiscard]] static auto is_task_item(const std::string& line) -> bool
    {
        const auto trimmed_start = find_content_start(line);
        if (trimmed_start == std::string::npos)
        {
            return false;
        }

        return has_checkbox_at(line, trimmed_start);
    }

    /// Check if a task item is checked.
    [[nodiscard]] static auto is_checked(const std::string& line) -> bool
    {
        const auto trimmed_start = find_content_start(line);
        if (trimmed_start == std::string::npos)
        {
            return false;
        }

        auto checkbox_pos = find_checkbox_pos(line, trimmed_start);
        if (checkbox_pos == std::string::npos)
        {
            return false;
        }

        return line[checkbox_pos + 1] == 'x' || line[checkbox_pos + 1] == 'X';
    }

    /// Toggle the checkbox state on a task list line.
    /// Returns the modified line. If the line is not a task item, returns it unchanged.
    [[nodiscard]] static auto toggle(const std::string& line) -> std::string
    {
        if (!is_task_item(line))
        {
            return line;
        }

        if (is_checked(line))
        {
            return set_checked(line, false);
        }
        return set_checked(line, true);
    }

    /// Set the checkbox to a specific state.
    /// Returns the modified line.
    [[nodiscard]] static auto set_checked(const std::string& line, bool checked) -> std::string
    {
        const auto trimmed_start = find_content_start(line);
        if (trimmed_start == std::string::npos)
        {
            return line;
        }

        auto checkbox_pos = find_checkbox_pos(line, trimmed_start);
        if (checkbox_pos == std::string::npos)
        {
            return line;
        }

        std::string result = line;
        // Replace the character inside [ ]
        result[checkbox_pos + 1] = checked ? 'x' : ' ';
        return result;
    }

    /// Count task items in a document.
    [[nodiscard]] static auto count_tasks(const std::string& document) -> TaskStats
    {
        TaskStats stats;
        std::istringstream stream(document);
        std::string line;

        while (std::getline(stream, line))
        {
            if (is_task_item(line))
            {
                ++stats.total;
                if (is_checked(line))
                {
                    ++stats.completed;
                }
            }
        }

        return stats;
    }

    /// Get the task text (content after the checkbox).
    [[nodiscard]] static auto get_task_text(const std::string& line) -> std::string
    {
        const auto trimmed_start = find_content_start(line);
        if (trimmed_start == std::string::npos)
        {
            return "";
        }

        auto checkbox_pos = find_checkbox_pos(line, trimmed_start);
        if (checkbox_pos == std::string::npos)
        {
            return "";
        }

        // Text starts after "] "
        auto text_start = checkbox_pos + 3;
        if (text_start < line.size())
        {
            return line.substr(text_start);
        }
        return "";
    }

private:
    /// Find the first non-whitespace position in a line.
    [[nodiscard]] static auto find_content_start(const std::string& line) -> std::size_t
    {
        return line.find_first_not_of(" \t");
    }

    /// Check if there's a checkbox marker at the given position.
    /// Expects: marker + space + [ + x_or_space + ]
    [[nodiscard]] static auto has_checkbox_at(const std::string& line, std::size_t content_start)
        -> bool
    {
        if (content_start + 4 >= line.size())
        {
            return false;
        }

        const char marker = line[content_start];
        if (marker != '-' && marker != '*' && marker != '+')
        {
            // Check for ordered list: 1. [ ] or 1) [ ]
            if (std::isdigit(static_cast<unsigned char>(marker)) == 0)
            {
                return false;
            }
            // Find the . or ) after digits
            std::size_t scan = content_start + 1;
            while (scan < line.size() && std::isdigit(static_cast<unsigned char>(line[scan])) != 0)
            {
                ++scan;
            }
            if (scan >= line.size() || (line[scan] != '.' && line[scan] != ')'))
            {
                return false;
            }
            ++scan; // Skip . or )
            if (scan >= line.size() || line[scan] != ' ')
            {
                return false;
            }
            ++scan; // Skip space
            if (scan + 2 >= line.size())
            {
                return false;
            }
            return line[scan] == '[' &&
                   (line[scan + 1] == ' ' || line[scan + 1] == 'x' || line[scan + 1] == 'X') &&
                   line[scan + 2] == ']';
        }

        if (line[content_start + 1] != ' ')
        {
            return false;
        }

        return line[content_start + 2] == '[' &&
               (line[content_start + 3] == ' ' || line[content_start + 3] == 'x' ||
                line[content_start + 3] == 'X') &&
               line[content_start + 4] == ']';
    }

    /// Find the position of the '[' in the checkbox pattern.
    [[nodiscard]] static auto find_checkbox_pos(const std::string& line, std::size_t content_start)
        -> std::size_t
    {
        const char marker = line[content_start];

        if (marker == '-' || marker == '*' || marker == '+')
        {
            // Unordered: - [ ] or * [ ] or + [ ]
            if (content_start + 4 < line.size() && line[content_start + 1] == ' ' &&
                line[content_start + 2] == '[')
            {
                return content_start + 2;
            }
        }
        else if (std::isdigit(static_cast<unsigned char>(marker)) != 0)
        {
            // Ordered: 1. [ ] or 1) [ ]
            std::size_t scan = content_start + 1;
            while (scan < line.size() && std::isdigit(static_cast<unsigned char>(line[scan])) != 0)
            {
                ++scan;
            }
            if (scan < line.size() && (line[scan] == '.' || line[scan] == ')'))
            {
                ++scan; // Skip . or )
                if (scan < line.size() && line[scan] == ' ')
                {
                    ++scan; // Skip space
                    if (scan < line.size() && line[scan] == '[')
                    {
                        return scan;
                    }
                }
            }
        }

        return std::string::npos;
    }
};

} // namespace markamp::core
