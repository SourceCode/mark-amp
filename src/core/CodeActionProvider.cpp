#include "CodeActionProvider.h"

#include <algorithm>
#include <regex>
#include <set>
#include <sstream>
#include <unordered_set>

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
    auto cur_line_text = get_line_text(content, start_line);
    auto link_actions = get_link_conversion_actions(cur_line_text, start_line, content);
    for (auto& action : link_actions)
    {
        actions.push_back(std::move(action));
    }

    // 5. Trailing whitespace removal
    auto ws_actions = get_trailing_whitespace_action(cur_line_text, start_line);
    for (auto& action : ws_actions)
    {
        actions.push_back(std::move(action));
    }

    return actions;
}

auto CodeActionProvider::provide_action_set(const std::string& content,
                                            int start_line,
                                            int start_char,
                                            int end_line,
                                            int end_char,
                                            const std::string& document_uri) const -> CodeActionSet
{
    CodeActionSet result;

    // Get all flat actions and categorize them
    auto all = provide_actions(content, start_line, start_char, end_line, end_char, document_uri);
    for (auto& action : all)
    {
        result.add(std::move(action));
    }

    // Add source-level actions (document-wide)
    auto organize = get_organize_links_action(content);
    for (auto& action : organize)
    {
        result.add(std::move(action));
    }

    auto format = get_format_document_action(content);
    for (auto& action : format)
    {
        result.add(std::move(action));
    }

    auto unused_refs = get_remove_unused_references_action(content);
    for (auto& action : unused_refs)
    {
        result.add(std::move(action));
    }

    // Query extension providers
    for (const auto& [provider_id, provider_fn] : extension_providers_)
    {
        auto ext_actions =
            provider_fn(content, start_line, start_char, end_line, end_char, document_uri);
        for (auto& action : ext_actions)
        {
            result.add(std::move(action));
        }
    }

    return result;
}

void CodeActionProvider::register_provider(const std::string& provider_id,
                                           ExtensionActionProvider provider)
{
    extension_providers_[provider_id] = std::move(provider);
}

void CodeActionProvider::unregister_provider(const std::string& provider_id)
{
    extension_providers_.erase(provider_id);
}

auto CodeActionProvider::extension_provider_count() const -> size_t
{
    return extension_providers_.size();
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

    const std::string heading_markers(static_cast<std::string::size_type>(new_level), '#');

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

    // Suppress unused parameter warnings
    (void)end_line;
    (void)end_char;

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
        const int correct_level = prev_level + 1;
        const std::string correct_markers(static_cast<std::string::size_type>(correct_level), '#');
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

    auto ref_count = count_reference_links(content);
    auto ref_label = std::to_string(ref_count + 1);

    CodeActionInfo action;
    action.title = "Convert to reference link";
    action.kind = CodeActionKind::kRefactor;

    CodeActionEdit edit1;
    edit1.start_line = line;
    edit1.start_char = static_cast<int>(bracket_pos);
    edit1.end_line = line;
    edit1.end_char = static_cast<int>(close_paren + 1);
    edit1.new_text = "[" + link_text + "][" + ref_label + "]";
    action.edits.push_back(edit1);

    CodeActionEdit edit2;
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

    auto last_non_ws = line_text.find_last_not_of(" \t");
    if (last_non_ws == std::string::npos || last_non_ws == line_text.size() - 1)
    {
        return actions;
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

auto CodeActionProvider::get_organize_links_action(const std::string& content)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // Parse all reference link definitions [label]: url
    struct RefLink
    {
        std::string label;
        std::string url;
        int line_number{0};
    };

    std::vector<RefLink> ref_links;
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line))
    {
        if (current_line.size() > 3 && current_line[0] == '[' && current_line[1] != '^')
        {
            auto close = current_line.find(']');
            if (close != std::string::npos && close + 1 < current_line.size() &&
                current_line[close + 1] == ':')
            {
                RefLink ref;
                ref.label = current_line.substr(1, close - 1);
                // Trim leading space after ':'
                auto url_start = close + 2;
                while (url_start < current_line.size() && current_line[url_start] == ' ')
                {
                    ++url_start;
                }
                ref.url = current_line.substr(url_start);
                ref.line_number = line_num;
                ref_links.push_back(std::move(ref));
            }
        }
        ++line_num;
    }

    if (ref_links.size() < 2)
    {
        return actions; // Only offer when there are 2+ refs to organize
    }

    // Check if already sorted
    bool already_sorted = true;
    for (size_t idx = 1; idx < ref_links.size(); ++idx)
    {
        if (ref_links[idx].label < ref_links[idx - 1].label)
        {
            already_sorted = false;
            break;
        }
    }

    if (already_sorted)
    {
        return actions;
    }

    // Sort alphabetically
    auto sorted = ref_links;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const RefLink& lhs, const RefLink& rhs) { return lhs.label < rhs.label; });

    // Build edits: replace each ref link line with the sorted version
    CodeActionInfo action;
    action.title = "Organize reference links";
    action.kind = CodeActionKind::kSource;

    for (size_t idx = 0; idx < ref_links.size(); ++idx)
    {
        CodeActionEdit edit;
        edit.start_line = ref_links[idx].line_number;
        edit.start_char = 0;
        edit.end_line = ref_links[idx].line_number;
        edit.end_char = static_cast<int>(get_line_text(content, ref_links[idx].line_number).size());
        edit.new_text = "[" + sorted[idx].label + "]: " + sorted[idx].url;
        action.edits.push_back(edit);
    }

    actions.push_back(std::move(action));
    return actions;
}

auto CodeActionProvider::get_format_document_action(const std::string& content)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // Check for formatting issues: consecutive blank lines
    bool has_issues = false;
    std::istringstream stream(content);
    std::string current_line;
    int consecutive_blanks = 0;

    while (std::getline(stream, current_line))
    {
        if (current_line.empty() || current_line.find_first_not_of(" \t") == std::string::npos)
        {
            ++consecutive_blanks;
            if (consecutive_blanks > 1)
            {
                has_issues = true;
                break;
            }
        }
        else
        {
            consecutive_blanks = 0;
        }
    }

    if (!has_issues)
    {
        return actions;
    }

    // Build formatted content: collapse consecutive blank lines to single blank
    std::string formatted;
    std::istringstream format_stream(content);
    consecutive_blanks = 0;

    while (std::getline(format_stream, current_line))
    {
        if (current_line.empty() || current_line.find_first_not_of(" \t") == std::string::npos)
        {
            ++consecutive_blanks;
            if (consecutive_blanks <= 1)
            {
                formatted += "\n";
            }
        }
        else
        {
            consecutive_blanks = 0;
            formatted += current_line + "\n";
        }
    }

    // Remove trailing newline if original didn't have one
    if (!content.empty() && content.back() != '\n' && !formatted.empty() &&
        formatted.back() == '\n')
    {
        formatted.pop_back();
    }

    if (formatted == content)
    {
        return actions;
    }

    // Count lines
    int total_lines = 0;
    std::istringstream count_stream(content);
    std::string count_line;
    while (std::getline(count_stream, count_line))
    {
        ++total_lines;
    }

    CodeActionInfo action;
    action.title = "Format document";
    action.kind = CodeActionKind::kSource;

    CodeActionEdit edit;
    edit.start_line = 0;
    edit.start_char = 0;
    edit.end_line = total_lines > 0 ? total_lines - 1 : 0;
    edit.end_char = static_cast<int>(get_line_text(content, edit.end_line).size());
    edit.new_text = formatted;
    action.edits.push_back(edit);

    actions.push_back(std::move(action));
    return actions;
}

auto CodeActionProvider::get_remove_unused_references_action(const std::string& content)
    -> std::vector<CodeActionInfo>
{
    std::vector<CodeActionInfo> actions;

    // Find all reference definitions [label]: url
    struct RefDef
    {
        std::string label;
        int line_number{0};
    };

    std::vector<RefDef> ref_defs;
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line))
    {
        if (current_line.size() > 3 && current_line[0] == '[' && current_line[1] != '^')
        {
            auto close = current_line.find(']');
            if (close != std::string::npos && close + 1 < current_line.size() &&
                current_line[close + 1] == ':')
            {
                RefDef def;
                def.label = current_line.substr(1, close - 1);
                def.line_number = line_num;
                ref_defs.push_back(std::move(def));
            }
        }
        ++line_num;
    }

    if (ref_defs.empty())
    {
        return actions;
    }

    // Find all reference usages [text][label]
    std::unordered_set<std::string> used_labels;
    std::istringstream usage_stream(content);
    while (std::getline(usage_stream, current_line))
    {
        // Look for [text][label] patterns
        std::string::size_type pos = 0;
        while (pos < current_line.size())
        {
            auto open_bracket = current_line.find("][", pos);
            if (open_bracket == std::string::npos)
            {
                break;
            }
            auto close_bracket = current_line.find(']', open_bracket + 2);
            if (close_bracket != std::string::npos)
            {
                auto label =
                    current_line.substr(open_bracket + 2, close_bracket - open_bracket - 2);
                used_labels.insert(label);
            }
            pos = open_bracket + 1;
        }
    }

    // Find unused references
    std::vector<int> unused_lines;
    for (const auto& def : ref_defs)
    {
        if (used_labels.find(def.label) == used_labels.end())
        {
            unused_lines.push_back(def.line_number);
        }
    }

    if (unused_lines.empty())
    {
        return actions;
    }

    CodeActionInfo action;
    action.title = "Remove " + std::to_string(unused_lines.size()) + " unused reference" +
                   (unused_lines.size() > 1 ? "s" : "");
    action.kind = CodeActionKind::kSource;

    // Remove lines in reverse order to preserve line numbers
    std::sort(unused_lines.rbegin(), unused_lines.rend());
    for (const int unused_line : unused_lines)
    {
        CodeActionEdit edit;
        edit.start_line = unused_line;
        edit.start_char = 0;
        edit.end_line = unused_line + 1; // Remove the entire line including newline
        edit.end_char = 0;
        edit.new_text = "";
        action.edits.push_back(edit);
    }

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
        if (current_line.size() > 3 && current_line[0] == '[')
        {
            auto close = current_line.find(']');
            if (close != std::string::npos && close + 1 < current_line.size() &&
                current_line[close + 1] == ':')
            {
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
