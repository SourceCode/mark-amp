/// @file ClipboardService.cpp
/// @brief V4 Phase 17 – Copy/Paste with Formatting implementation.

#include "core/ClipboardService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

ClipboardService::ClipboardService(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Extract attribute from HTML tag
// ============================================================================

auto ClipboardService::extract_attr(const std::string& attrs, const std::string& name)
    -> std::string
{
    // Match name="value" or name='value'
    std::regex attr_re(name + R"(\s*=\s*["']([^"']*)["'])");
    std::smatch match;
    if (std::regex_search(attrs, match, attr_re))
    {
        return match[1].str();
    }
    return {};
}

// ============================================================================
// Convert a single HTML tag to Markdown
// ============================================================================

auto ClipboardService::convert_tag(const std::string& tag,
                                   const std::string& inner,
                                   const std::string& attrs) -> std::string
{
    if (tag == "strong" || tag == "b")
    {
        return "**" + inner + "**";
    }
    if (tag == "em" || tag == "i")
    {
        return "*" + inner + "*";
    }
    if (tag == "code")
    {
        return "`" + inner + "`";
    }
    if (tag == "a")
    {
        auto href = extract_attr(attrs, "href");
        if (href.empty())
        {
            return inner;
        }
        return "[" + inner + "](" + href + ")";
    }
    if (tag == "img")
    {
        auto src = extract_attr(attrs, "src");
        auto alt = extract_attr(attrs, "alt");
        return "![" + alt + "](" + src + ")";
    }
    if (tag == "h1")
    {
        return "# " + inner + "\n";
    }
    if (tag == "h2")
    {
        return "## " + inner + "\n";
    }
    if (tag == "h3")
    {
        return "### " + inner + "\n";
    }
    if (tag == "h4")
    {
        return "#### " + inner + "\n";
    }
    if (tag == "h5")
    {
        return "##### " + inner + "\n";
    }
    if (tag == "h6")
    {
        return "###### " + inner + "\n";
    }
    if (tag == "br")
    {
        return "\n";
    }
    if (tag == "hr")
    {
        return "\n---\n";
    }
    if (tag == "p")
    {
        return inner + "\n\n";
    }
    if (tag == "blockquote")
    {
        return "> " + inner + "\n";
    }
    if (tag == "li")
    {
        return "- " + inner + "\n";
    }
    if (tag == "ul" || tag == "ol")
    {
        return inner;
    }
    if (tag == "pre")
    {
        return "```\n" + inner + "\n```\n";
    }

    // Unknown tag: return inner content
    return inner;
}

// ============================================================================
// HTML to Markdown
// ============================================================================

auto ClipboardService::html_to_markdown(const std::string& html) const -> std::string
{
    std::string result = html;

    // Process self-closing tags first (img, br, hr)
    {
        std::regex self_close(R"(<(img|br|hr)\s*([^>]*?)\s*/?>)", std::regex::icase);
        std::string built;
        auto search_begin = result.cbegin();
        std::sregex_iterator iter(result.cbegin(), result.cend(), self_close);
        const std::sregex_iterator end_iter;
        for (; iter != end_iter; ++iter)
        {
            built.append(search_begin, (*iter)[0].first);
            auto tag = (*iter)[1].str();
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            built += convert_tag(tag, "", (*iter)[2].str());
            search_begin = (*iter)[0].second;
        }
        built.append(search_begin, result.cend());
        result = built;
    }

    // Process paired tags from inside out (repeat to handle nesting)
    std::regex paired(R"(<(\w+)(\s[^>]*)?>([^<]*)</\1>)", std::regex::icase);
    for (int pass = 0; pass < 5; ++pass)
    {
        std::string prev = result;
        std::string built;
        auto search_begin = result.cbegin();
        std::sregex_iterator iter(result.cbegin(), result.cend(), paired);
        const std::sregex_iterator end_iter;
        for (; iter != end_iter; ++iter)
        {
            built.append(search_begin, (*iter)[0].first);
            auto tag = (*iter)[1].str();
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            auto attrs = (*iter)[2].matched ? (*iter)[2].str() : std::string{};
            built += convert_tag(tag, (*iter)[3].str(), attrs);
            search_begin = (*iter)[0].second;
        }
        built.append(search_begin, result.cend());
        result = built;
        if (result == prev)
        {
            break; // No more changes
        }
    }

    // Strip any remaining tags
    result = strip_html_tags(result);

    // Collapse extra newlines
    std::regex multi_newline(R"(\n{3,})");
    result = std::regex_replace(result, multi_newline, "\n\n");

    // Trim
    while (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    return result;
}

// ============================================================================
// CSV to Markdown table
// ============================================================================

auto ClipboardService::csv_to_markdown_table(const std::string& csv) const -> std::string
{
    std::istringstream stream(csv);
    std::string line;
    std::vector<std::vector<std::string>> rows;

    while (std::getline(stream, line))
    {
        if (line.empty())
        {
            continue;
        }
        std::vector<std::string> cells;
        std::istringstream cell_stream(line);
        std::string cell;
        while (std::getline(cell_stream, cell, ','))
        {
            // Trim whitespace and quotes
            while (!cell.empty() && (cell.front() == ' ' || cell.front() == '"'))
            {
                cell.erase(cell.begin());
            }
            while (!cell.empty() && (cell.back() == ' ' || cell.back() == '"'))
            {
                cell.pop_back();
            }
            cells.push_back(cell);
        }
        rows.push_back(std::move(cells));
    }

    if (rows.empty())
    {
        return {};
    }

    std::string result;
    // Header row
    result += "| ";
    for (size_t col = 0; col < rows[0].size(); ++col)
    {
        if (col > 0)
        {
            result += " | ";
        }
        result += rows[0][col];
    }
    result += " |\n";

    // Separator
    result += "|";
    for (size_t col = 0; col < rows[0].size(); ++col)
    {
        result += " --- |";
    }
    result += "\n";

    // Data rows
    for (size_t row = 1; row < rows.size(); ++row)
    {
        result += "| ";
        for (size_t col = 0; col < rows[row].size(); ++col)
        {
            if (col > 0)
            {
                result += " | ";
            }
            result += rows[row][col];
        }
        result += " |\n";
    }

    return result;
}

// ============================================================================
// URL to Markdown link
// ============================================================================

auto ClipboardService::url_to_markdown_link(const std::string& url) const -> std::string
{
    // Check if it's an image URL
    static const std::vector<std::string> image_extensions = {
        ".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp", ".bmp"};

    std::string lower_url = url;
    std::transform(lower_url.begin(), lower_url.end(), lower_url.begin(), ::tolower);

    for (const auto& ext : image_extensions)
    {
        // Check extension before any query params
        auto question_pos = lower_url.find('?');
        auto check_url =
            (question_pos != std::string::npos) ? lower_url.substr(0, question_pos) : lower_url;

        if (check_url.size() >= ext.size() &&
            check_url.substr(check_url.size() - ext.size()) == ext)
        {
            return "![image](" + url + ")";
        }
    }

    return "[link](" + url + ")";
}

// ============================================================================
// Strip HTML tags
// ============================================================================

auto ClipboardService::strip_html_tags(const std::string& html) -> std::string
{
    std::string result;
    result.reserve(html.size());
    bool in_tag = false;

    for (char chr : html)
    {
        if (chr == '<')
        {
            in_tag = true;
            continue;
        }
        if (chr == '>')
        {
            in_tag = false;
            continue;
        }
        if (!in_tag)
        {
            result += chr;
        }
    }

    return result;
}

// ============================================================================
// Detect content type
// ============================================================================

auto ClipboardService::detect_content_type(const PasteContent& content) const -> PasteContentType
{
    if (!content.image_data.empty())
    {
        return PasteContentType::kImage;
    }
    if (!content.html.empty())
    {
        return PasteContentType::kHtml;
    }
    if (!content.url.empty())
    {
        return PasteContentType::kUrl;
    }
    if (!content.file_paths.empty())
    {
        return PasteContentType::kFilePaths;
    }
    if (!content.text.empty())
    {
        // Check if it looks like CSV
        if (content.text.find(',') != std::string::npos &&
            content.text.find('\n') != std::string::npos)
        {
            return PasteContentType::kCsvTable;
        }
        return PasteContentType::kPlainText;
    }
    return PasteContentType::kUnknown;
}

// ============================================================================
// Convert to Markdown (full pipeline)
// ============================================================================

auto ClipboardService::convert_to_markdown(const PasteContent& content) const -> PasteResult
{
    PasteResult result;
    result.success = true;

    auto type = detect_content_type(content);

    switch (type)
    {
        case PasteContentType::kHtml:
            result.markdown = html_to_markdown(content.html);
            break;
        case PasteContentType::kUrl:
            result.markdown = url_to_markdown_link(content.url);
            break;
        case PasteContentType::kCsvTable:
            result.markdown = csv_to_markdown_table(content.text);
            break;
        case PasteContentType::kPlainText:
            result.markdown = content.text;
            break;
        case PasteContentType::kImage:
            result.markdown = "![pasted image]()";
            break;
        case PasteContentType::kFilePaths:
            for (const auto& path : content.file_paths)
            {
                result.markdown += "[" + path + "](" + path + ")\n";
            }
            break;
        default:
            result.success = false;
            result.error = "Unknown content type";
            break;
    }

    return result;
}

} // namespace markamp::core
