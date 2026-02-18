#include "CodeActionProvider.h"

#include <sstream>

namespace markamp::core
{

CodeActionProvider::CodeActionProvider(DiagnosticsService& diagnostics_service)
    : diagnostics_service_(diagnostics_service)
{
}

auto CodeActionProvider::provide_actions(const std::string& content,
                                         int start_line,
                                         int start_char,
                                         int end_line,
                                         int end_char,
                                         const std::string& document_uri) const
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // 1. Diagnostic quick-fixes
    auto diag_fixes = get_diagnostic_fixes(start_line, end_line, document_uri);
    for (auto& fix : diag_fixes)
    {
        actions.push_back(std::move(fix));
    }

    // 2. Extract to heading (when text is selected across lines or has content)
    if (start_line != end_line || start_char != end_char)
    {
        auto extract_actions =
            get_extract_heading_action(content, start_line, start_char, end_line, end_char);
        for (auto& action : extract_actions)
        {
            actions.push_back(std::move(action));
        }
    }

    // 3. Heading hierarchy fixes (when cursor is on a heading)
    auto heading_fixes = get_heading_hierarchy_fixes(content, start_line);
    for (auto& fix : heading_fixes)
    {
        actions.push_back(std::move(fix));
    }

    // 4. Link conversion actions (when cursor is on a link)
    const auto line_text = get_line_text(content, start_line);
    auto link_actions = get_link_conversion_actions(line_text, start_line, content);
    for (auto& action : link_actions)
    {
        actions.push_back(std::move(action));
    }

    // 5. Trailing whitespace removal
    auto ws_actions = get_trailing_whitespace_action(line_text, start_line);
    for (auto& action : ws_actions)
    {
        actions.push_back(std::move(action));
    }

    return actions;
}

auto CodeActionProvider::get_diagnostic_fixes(int start_line,
                                              int end_line,
                                              const std::string& document_uri) const
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> fixes;

    if (document_uri.empty())
    {
        return fixes;
    }

    const auto& diagnostics = diagnostics_service_.get(document_uri);
    for (const auto& diag : diagnostics)
    {
        // Check if diagnostic overlaps with the selection range
        if (diag.range.start.line > end_line || diag.range.end.line < start_line)
        {
            continue;
        }

        CodeActionInfo fix;
        fix.kind = CodeActionKind::kQuickFix;
        fix.diagnostic_code = diag.code;
        fix.is_preferred = true;

        if (diag.code == "trailing-whitespace")
        {
            fix.title = "Remove trailing whitespace";
            // The edit trims the line
            CodeActionEdit edit;
            edit.start_line = diag.range.start.line;
            edit.start_char = diag.range.start.character;
            edit.end_line = diag.range.end.line;
            edit.end_char = diag.range.end.character;
            edit.new_text = "";
            fix.edits.push_back(edit);
        }
        else if (diag.code == "heading-skip-level")
        {
            fix.title = "Fix heading level to " + diag.message;
            // Suggest replacing heading markers
            CodeActionEdit edit;
            edit.start_line = diag.range.start.line;
            edit.start_char = 0;
            edit.end_line = diag.range.start.line;
            edit.end_char = diag.range.start.character;
            edit.new_text = ""; // Will be filled based on context
            fix.edits.push_back(edit);
        }
        else
        {
            fix.title = "Fix: " + diag.message;
        }

        fixes.push_back(std::move(fix));
    }

    return fixes;
}

auto CodeActionProvider::get_extract_heading_action(const std::string& content,
                                                    int start_line,
                                                    int start_char,
                                                    int end_line,
                                                    int end_char) -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // Determine the heading level to use — find the parent heading level
    int parent_level = 0;
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line) && line_num < start_line)
    {
        if (!current_line.empty() && current_line[0] == '#')
        {
            int level = 0;
            while (level < static_cast<int>(current_line.size()) &&
                   current_line[static_cast<std::string::size_type>(level)] == '#')
            {
                ++level;
            }
            if (level <= 6)
            {
                parent_level = level;
            }
        }
        ++line_num;
    }

    int new_level = parent_level + 1;
    if (new_level > 6)
    {
        new_level = 6;
    }

    std::string heading_markers(static_cast<std::string::size_type>(new_level), '#');

    CodeActionInfo action;
    action.title = "Extract to H" + std::to_string(new_level) + " heading";
    action.kind = CodeActionKind::kRefactorExtract;

    // Insert heading before the selected text
    CodeActionEdit edit;
    edit.start_line = start_line;
    edit.start_char = start_char;
    edit.end_line = start_line;
    edit.end_char = start_char;
    edit.new_text = heading_markers + " New Section\n\n";
    action.edits.push_back(edit);

    actions.push_back(std::move(action));
    return actions;
}

auto CodeActionProvider::get_heading_hierarchy_fixes(const std::string& content, int line)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> fixes;

    auto line_text = get_line_text(content, line);
    if (line_text.empty() || line_text[0] != '#')
    {
        return fixes;
    }

    int current_level = 0;
    while (current_level < static_cast<int>(line_text.size()) &&
           line_text[static_cast<std::string::size_type>(current_level)] == '#')
    {
        ++current_level;
    }

    if (current_level > 6 || current_level >= static_cast<int>(line_text.size()) ||
        line_text[static_cast<std::string::size_type>(current_level)] != ' ')
    {
        return fixes;
    }

    // Find the previous heading level
    int prev_level = 0;
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line) && line_num < line)
    {
        if (!current_line.empty() && current_line[0] == '#')
        {
            int level = 0;
            while (level < static_cast<int>(current_line.size()) &&
                   current_line[static_cast<std::string::size_type>(level)] == '#')
            {
                ++level;
            }
            if (level <= 6)
            {
                prev_level = level;
            }
        }
        ++line_num;
    }

    // Check for skip-level (e.g., H1 → H3)
    if (current_level > prev_level + 1 && prev_level > 0)
    {
        int correct_level = prev_level + 1;
        std::string correct_markers(static_cast<std::string::size_type>(correct_level), '#');
        auto heading_content = line_text.substr(static_cast<std::string::size_type>(current_level));

        CodeActionInfo fix;
        fix.title = "Fix heading level: H" + std::to_string(current_level) + " → H" +
                    std::to_string(correct_level);
        fix.kind = CodeActionKind::kQuickFix;
        fix.is_preferred = true;

        CodeActionEdit edit;
        edit.start_line = line;
        edit.start_char = 0;
        edit.end_line = line;
        edit.end_char = static_cast<int>(line_text.size());
        edit.new_text = correct_markers + heading_content;
        fix.edits.push_back(edit);

        fixes.push_back(std::move(fix));
    }

    return fixes;
}

auto CodeActionProvider::get_link_conversion_actions(const std::string& line_text,
                                                     int line,
                                                     const std::string& content)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // Find inline link [text](url)
    auto bracket_pos = line_text.find('[');
    if (bracket_pos == std::string::npos)
    {
        return actions;
    }

    // Skip images and wiki-links
    if (bracket_pos > 0 && line_text[bracket_pos - 1] == '!')
    {
        return actions;
    }
    if (bracket_pos > 0 && line_text[bracket_pos - 1] == '[')
    {
        return actions;
    }

    auto close_bracket = line_text.find(']', bracket_pos + 1);
    if (close_bracket == std::string::npos || close_bracket + 1 >= line_text.size() ||
        line_text[close_bracket + 1] != '(')
    {
        return actions;
    }

    auto close_paren = line_text.find(')', close_bracket + 2);
    if (close_paren == std::string::npos)
    {
        return actions;
    }

    auto link_text = line_text.substr(bracket_pos + 1, close_bracket - bracket_pos - 1);
    auto url = line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);

    // Generate a reference label
    auto ref_count = count_reference_links(content);
    auto ref_label = std::to_string(ref_count + 1);

    CodeActionInfo action;
    action.title = "Convert to reference link";
    action.kind = CodeActionKind::kRefactor;

    // Replace inline link with reference
    CodeActionEdit edit1;
    edit1.start_line = line;
    edit1.start_char = static_cast<int>(bracket_pos);
    edit1.end_line = line;
    edit1.end_char = static_cast<int>(close_paren + 1);
    edit1.new_text = "[" + link_text + "][" + ref_label + "]";
    action.edits.push_back(edit1);

    // Append reference definition at end of document
    CodeActionEdit edit2;
    // Count total lines
    int total_lines = 0;
    std::istringstream stream(content);
    std::string tmp;
    while (std::getline(stream, tmp))
    {
        ++total_lines;
    }
    edit2.start_line = total_lines;
    edit2.start_char = 0;
    edit2.end_line = total_lines;
    edit2.end_char = 0;
    edit2.new_text = "\n[" + ref_label + "]: " + url + "\n";
    action.edits.push_back(edit2);

    actions.push_back(std::move(action));
    return actions;
}

auto CodeActionProvider::get_trailing_whitespace_action(const std::string& line_text, int line)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    if (line_text.empty())
    {
        return actions;
    }

    // Check for trailing whitespace (but not markdown line breaks: two trailing spaces)
    auto last_non_ws = line_text.find_last_not_of(" \t");
    if (last_non_ws == std::string::npos || last_non_ws == line_text.size() - 1)
    {
        return actions; // No trailing whitespace
    }

    auto trailing_count = line_text.size() - last_non_ws - 1;
    if (trailing_count == 2)
    {
        return actions; // Two trailing spaces = intentional line break
    }

    CodeActionInfo action;
    action.title = "Remove trailing whitespace";
    action.kind = CodeActionKind::kSourceFixAll;

    CodeActionEdit edit;
    edit.start_line = line;
    edit.start_char = static_cast<int>(last_non_ws + 1);
    edit.end_line = line;
    edit.end_char = static_cast<int>(line_text.size());
    edit.new_text = "";
    action.edits.push_back(edit);

    actions.push_back(std::move(action));
    return actions;
}

auto CodeActionProvider::get_line_text(const std::string& content, int line) -> std::string
{
    std::istringstream stream(content);
    std::string current_line;
    int current = 0;
    while (std::getline(stream, current_line))
    {
        if (current == line)
        {
            return current_line;
        }
        ++current;
    }
    return "";
}

auto CodeActionProvider::count_reference_links(const std::string& content) -> int
{
    int count = 0;
    std::istringstream stream(content);
    std::string current_line;
    while (std::getline(stream, current_line))
    {
        // Reference link definitions start with [label]: url
        if (current_line.size() > 3 && current_line[0] == '[')
        {
            auto close = current_line.find(']');
            if (close != std::string::npos && close + 1 < current_line.size() &&
                current_line[close + 1] == ':')
            {
                // Make sure it's not a footnote [^id]:
                if (current_line[1] != '^')
                {
                    ++count;
                }
            }
        }
    }
    return count;
}

} // namespace markamp::core
