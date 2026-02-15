#include "CardFactory.h"

#include <sstream>

namespace markamp::core
{

auto CardFactory::create_card_content(const std::string& block_id,
                                      const std::string& markdown) const -> CardContent
{
    CardContent content;
    content.source_block_id = block_id;
    content.format = detect_format(markdown);

    switch (content.format)
    {
        case CardFormat::MarkBased:
        {
            const auto [front, back] = split_by_marks(markdown);
            content.front = front;
            content.back = back;
            break;
        }
        case CardFormat::ListBased:
        {
            const auto [front, back] = split_by_list(markdown);
            content.front = front;
            content.back = back;
            break;
        }
        case CardFormat::HeadingBased:
        {
            const auto [front, back] = split_by_heading(markdown);
            content.front = front;
            content.back = back;
            break;
        }
        case CardFormat::FullBlock:
        default:
            content.front = markdown;
            content.back = markdown;
            break;
    }

    return content;
}

auto CardFactory::detect_format(const std::string& markdown) const -> CardFormat
{
    if (has_mark_syntax(markdown))
    {
        return CardFormat::MarkBased;
    }
    if (has_list_definition(markdown))
    {
        return CardFormat::ListBased;
    }
    if (has_heading_structure(markdown))
    {
        return CardFormat::HeadingBased;
    }
    return CardFormat::FullBlock;
}

auto CardFactory::has_mark_syntax(const std::string& markdown) const -> bool
{
    // Look for ==...== (SiYuan-style mark syntax)
    static const std::regex mark_pattern(R"(==.+?==)");
    return std::regex_search(markdown, mark_pattern);
}

auto CardFactory::has_list_definition(const std::string& markdown) const -> bool
{
    // Look for "term :: definition" pattern in list items
    static const std::regex list_pattern(R"(^\s*[-*+]\s+.+?\s*::\s*.+$)", std::regex::multiline);
    return std::regex_search(markdown, list_pattern);
}

auto CardFactory::has_heading_structure(const std::string& markdown) const -> bool
{
    // Starts with a markdown heading (# title)
    static const std::regex heading_pattern(R"(^#{1,6}\s+.+$)", std::regex::multiline);
    return std::regex_search(markdown, heading_pattern);
}

auto CardFactory::split_by_marks(const std::string& markdown) const
    -> std::pair<std::string, std::string>
{
    // Front = text with marks hidden (cloze deletion front)
    // Back = text with marks revealed
    static const std::regex mark_pattern(R"(==(.+?)==)");

    // Front: replace marked text with blanks
    const auto front = std::regex_replace(markdown, mark_pattern, "[...]");

    // Back: extract marked text
    std::string back;
    std::sregex_iterator iter(markdown.begin(), markdown.end(), mark_pattern);
    std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        if (!back.empty())
            back += ", ";
        back += (*iter)[1].str();
    }

    if (back.empty())
    {
        back = markdown;
    }

    return {front, back};
}

auto CardFactory::split_by_list(const std::string& markdown) const
    -> std::pair<std::string, std::string>
{
    // Split on " :: " — left side is front, right side is back
    const auto sep_pos = markdown.find(" :: ");
    if (sep_pos == std::string::npos)
    {
        return {markdown, markdown};
    }

    auto front = markdown.substr(0, sep_pos);
    auto back = markdown.substr(sep_pos + 4); // skip " :: "

    // Strip leading list markers from front
    static const std::regex list_marker(R"(^\s*[-*+]\s+)");
    front = std::regex_replace(front, list_marker, "");

    return {front, back};
}

auto CardFactory::split_by_heading(const std::string& markdown) const
    -> std::pair<std::string, std::string>
{
    // Find the first newline after the heading
    const auto newline_pos = markdown.find('\n');
    if (newline_pos == std::string::npos)
    {
        return {markdown, ""};
    }

    auto heading = markdown.substr(0, newline_pos);
    auto body = markdown.substr(newline_pos + 1);

    // Strip heading markers (# )
    static const std::regex heading_marker(R"(^#{1,6}\s+)");
    heading = std::regex_replace(heading, heading_marker, "");

    // Trim leading whitespace from body
    const auto first_non_ws = body.find_first_not_of(" \t\n\r");
    if (first_non_ws != std::string::npos)
    {
        body = body.substr(first_non_ws);
    }

    return {heading, body};
}

auto CardFactory::extract_mark_content(const std::string& text) -> std::string
{
    static const std::regex mark_pattern(R"(==(.+?)==)");
    std::string result;
    std::sregex_iterator iter(text.begin(), text.end(), mark_pattern);
    std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        if (!result.empty())
            result += ", ";
        result += (*iter)[1].str();
    }
    return result;
}

} // namespace markamp::core
