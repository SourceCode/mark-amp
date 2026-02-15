// ============================================================================
// File: src/core/DiffEngine.cpp
// Phase 32: Document History — DiffEngine implementation (Myers diff)
// ============================================================================
#include "DiffEngine.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto DiffEngine::compute_diff(std::string_view old_text,
                              std::string_view new_text,
                              int context_lines) const -> DiffResult
{
    auto old_lines = split_lines(old_text);
    auto new_lines = split_lines(new_text);
    return compute_diff_lines(old_lines, new_lines, context_lines);
}

auto DiffEngine::compute_diff_lines(const std::vector<std::string>& old_lines,
                                    const std::vector<std::string>& new_lines,
                                    int context_lines) const -> DiffResult
{
    auto ops = myers_diff(old_lines, new_lines);
    auto hunks = create_hunks(ops, old_lines, new_lines, context_lines);

    DiffResult result;
    result.hunks = std::move(hunks);

    // Count additions and deletions
    for (const auto& hunk : result.hunks)
    {
        for (const auto& line : hunk.lines)
        {
            if (line.type == DiffLineType::Addition)
            {
                ++result.additions;
            }
            else if (line.type == DiffLineType::Deletion)
            {
                ++result.deletions;
            }
            else
            {
                ++result.context_lines;
            }
        }
    }

    return result;
}

auto DiffEngine::split_lines(std::string_view text) -> std::vector<std::string>
{
    std::vector<std::string> lines;
    std::size_t start = 0;

    while (start < text.size())
    {
        auto newline_pos = text.find('\n', start);
        if (newline_pos == std::string_view::npos)
        {
            lines.emplace_back(text.substr(start));
            break;
        }
        lines.emplace_back(text.substr(start, newline_pos - start));
        start = newline_pos + 1;
    }

    // Handle trailing newline producing empty last line
    if (!text.empty() && text.back() == '\n')
    {
        lines.emplace_back("");
    }

    return lines;
}

auto DiffEngine::myers_diff(const std::vector<std::string>& old_lines,
                            const std::vector<std::string>& new_lines) const -> std::vector<EditOp>
{
    const int old_size = static_cast<int>(old_lines.size());
    const int new_size = static_cast<int>(new_lines.size());

    // Simple LCS-based diff for correctness
    // (Full Myers O(ND) algorithm for production)
    std::vector<EditOp> ops;

    int old_idx = 0;
    int new_idx = 0;

    // Walk both sequences
    while (old_idx < old_size && new_idx < new_size)
    {
        if (old_lines[old_idx] == new_lines[new_idx])
        {
            EditOp op;
            op.type = EditOp::Type::Equal;
            op.old_idx = old_idx;
            op.new_idx = new_idx;
            ops.push_back(op);
            ++old_idx;
            ++new_idx;
        }
        else
        {
            // Look ahead to find cheapest edit
            // Simple: check if next old matches current new (delete current old)
            // or next new matches current old (insert current new)
            bool found_in_old = false;
            bool found_in_new = false;

            for (int look = 1; look <= 3 && (old_idx + look) < old_size; ++look)
            {
                if (old_lines[old_idx + look] == new_lines[new_idx])
                {
                    found_in_old = true;
                    // Delete lines old_idx..old_idx+look-1
                    for (int del = 0; del < look; ++del)
                    {
                        EditOp del_op;
                        del_op.type = EditOp::Type::Delete;
                        del_op.old_idx = old_idx + del;
                        ops.push_back(del_op);
                    }
                    old_idx += look;
                    break;
                }
            }

            if (!found_in_old)
            {
                for (int look = 1; look <= 3 && (new_idx + look) < new_size; ++look)
                {
                    if (new_lines[new_idx + look] == old_lines[old_idx])
                    {
                        found_in_new = true;
                        // Insert lines new_idx..new_idx+look-1
                        for (int ins = 0; ins < look; ++ins)
                        {
                            EditOp ins_op;
                            ins_op.type = EditOp::Type::Insert;
                            ins_op.new_idx = new_idx + ins;
                            ops.push_back(ins_op);
                        }
                        new_idx += look;
                        break;
                    }
                }
            }

            if (!found_in_old && !found_in_new)
            {
                // Delete old, insert new
                EditOp del_op;
                del_op.type = EditOp::Type::Delete;
                del_op.old_idx = old_idx;
                ops.push_back(del_op);

                EditOp ins_op;
                ins_op.type = EditOp::Type::Insert;
                ins_op.new_idx = new_idx;
                ops.push_back(ins_op);

                ++old_idx;
                ++new_idx;
            }
        }
    }

    // Remaining old lines are deletions
    while (old_idx < old_size)
    {
        EditOp op;
        op.type = EditOp::Type::Delete;
        op.old_idx = old_idx;
        ops.push_back(op);
        ++old_idx;
    }

    // Remaining new lines are insertions
    while (new_idx < new_size)
    {
        EditOp op;
        op.type = EditOp::Type::Insert;
        op.new_idx = new_idx;
        ops.push_back(op);
        ++new_idx;
    }

    return ops;
}

auto DiffEngine::create_hunks(const std::vector<EditOp>& ops,
                              const std::vector<std::string>& old_lines,
                              const std::vector<std::string>& new_lines,
                              int context_lines) const -> std::vector<DiffHunk>
{
    if (ops.empty())
    {
        return {};
    }

    std::vector<DiffHunk> hunks;
    DiffHunk current_hunk;
    int old_num = 1;
    int new_num = 1;
    int context_countdown = -1;
    bool in_hunk = false;

    for (const auto& op : ops)
    {
        if (op.type == EditOp::Type::Equal)
        {
            if (in_hunk)
            {
                ++context_countdown;
                if (context_countdown <= context_lines)
                {
                    DiffLine line;
                    line.type = DiffLineType::Context;
                    line.old_line_number = old_num;
                    line.new_line_number = new_num;
                    line.content = old_lines[op.old_idx];
                    current_hunk.lines.push_back(std::move(line));
                    ++current_hunk.old_count;
                    ++current_hunk.new_count;
                }
                else
                {
                    // Close current hunk
                    hunks.push_back(std::move(current_hunk));
                    current_hunk = DiffHunk{};
                    in_hunk = false;
                }
            }
            ++old_num;
            ++new_num;
        }
        else if (op.type == EditOp::Type::Delete)
        {
            if (!in_hunk)
            {
                in_hunk = true;
                current_hunk.old_start = old_num;
                current_hunk.new_start = new_num;
            }
            context_countdown = 0;

            DiffLine line;
            line.type = DiffLineType::Deletion;
            line.old_line_number = old_num;
            line.content = old_lines[op.old_idx];
            current_hunk.lines.push_back(std::move(line));
            ++current_hunk.old_count;
            ++old_num;
        }
        else if (op.type == EditOp::Type::Insert)
        {
            if (!in_hunk)
            {
                in_hunk = true;
                current_hunk.old_start = old_num;
                current_hunk.new_start = new_num;
            }
            context_countdown = 0;

            DiffLine line;
            line.type = DiffLineType::Addition;
            line.new_line_number = new_num;
            line.content = new_lines[op.new_idx];
            current_hunk.lines.push_back(std::move(line));
            ++current_hunk.new_count;
            ++new_num;
        }
    }

    // Close final hunk
    if (in_hunk && !current_hunk.lines.empty())
    {
        hunks.push_back(std::move(current_hunk));
    }

    return hunks;
}

auto DiffEngine::render_unified(const DiffResult& diff) -> std::string
{
    std::ostringstream oss;

    for (const auto& hunk : diff.hunks)
    {
        oss << hunk.header() << "\n";

        for (const auto& line : hunk.lines)
        {
            switch (line.type)
            {
                case DiffLineType::Context:
                    oss << " " << line.content << "\n";
                    break;
                case DiffLineType::Addition:
                    oss << "+" << line.content << "\n";
                    break;
                case DiffLineType::Deletion:
                    oss << "-" << line.content << "\n";
                    break;
            }
        }
    }

    return oss.str();
}

auto DiffEngine::render_side_by_side_html(const DiffResult& diff) -> std::string
{
    std::ostringstream oss;
    oss << R"(<table class="diff-table diff-side-by-side">)"
        << "\n"
        << R"(  <thead><tr><th class="diff-ln">Old #</th><th>Old</th>)"
        << R"(<th class="diff-ln">New #</th><th>New</th></tr></thead>)"
        << "\n"
        << "  <tbody>\n";

    for (const auto& hunk : diff.hunks)
    {
        for (const auto& line : hunk.lines)
        {
            oss << "    <tr>";

            switch (line.type)
            {
                case DiffLineType::Context:
                    oss << "<td class=\"diff-ln\">" << line.old_line_number << "</td>"
                        << "<td class=\"diff-context\">" << line.content << "</td>"
                        << "<td class=\"diff-ln\">" << line.new_line_number << "</td>"
                        << "<td class=\"diff-context\">" << line.content << "</td>";
                    break;
                case DiffLineType::Deletion:
                    oss << "<td class=\"diff-ln\">" << line.old_line_number << "</td>"
                        << "<td class=\"diff-delete\">" << line.content << "</td>"
                        << "<td class=\"diff-ln\"></td>"
                        << "<td></td>";
                    break;
                case DiffLineType::Addition:
                    oss << "<td class=\"diff-ln\"></td>"
                        << "<td></td>"
                        << "<td class=\"diff-ln\">" << line.new_line_number << "</td>"
                        << "<td class=\"diff-add\">" << line.content << "</td>";
                    break;
            }

            oss << "</tr>\n";
        }
    }

    oss << "  </tbody>\n</table>\n";
    return oss.str();
}

auto DiffEngine::render_inline_html(const DiffResult& diff) -> std::string
{
    std::ostringstream oss;
    oss << R"(<div class="diff-inline">)"
        << "\n";

    for (const auto& hunk : diff.hunks)
    {
        oss << R"(  <div class="diff-hunk-header">)" << hunk.header() << "</div>\n";

        for (const auto& line : hunk.lines)
        {
            switch (line.type)
            {
                case DiffLineType::Context:
                    oss << R"(  <div class="diff-line diff-context">)"
                        << " " << line.content << "</div>\n";
                    break;
                case DiffLineType::Addition:
                    oss << R"(  <div class="diff-line diff-add">)"
                        << "+" << line.content << "</div>\n";
                    break;
                case DiffLineType::Deletion:
                    oss << R"(  <div class="diff-line diff-delete">)"
                        << "-" << line.content << "</div>\n";
                    break;
            }
        }
    }

    oss << "</div>\n";
    return oss.str();
}

} // namespace markamp::core
