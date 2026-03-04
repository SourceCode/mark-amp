#include "RichDescriptionRenderer.h"

namespace markamp::ui
{

auto RichDescriptionRenderer::parse(const std::string& description)
    -> std::vector<DescriptionSegment>
{
    std::vector<DescriptionSegment> segments;
    if (description.empty())
    {
        return segments;
    }

    std::string current_text;
    std::size_t pos = 0;

    while (pos < description.size())
    {
        // Check for Markdown link: [text](url)
        if (description[pos] == '[')
        {
            auto close_bracket = description.find(']', pos + 1);
            if (close_bracket != std::string::npos && close_bracket + 1 < description.size() &&
                description[close_bracket + 1] == '(')
            {
                auto close_paren = description.find(')', close_bracket + 2);
                if (close_paren != std::string::npos)
                {
                    // Flush current text
                    if (!current_text.empty())
                    {
                        segments.push_back({DescriptionSegmentType::kText, current_text, ""});
                        current_text.clear();
                    }

                    auto link_text = description.substr(pos + 1, close_bracket - pos - 1);
                    auto link_url =
                        description.substr(close_bracket + 2, close_paren - close_bracket - 2);
                    segments.push_back({DescriptionSegmentType::kLink, link_text, link_url});
                    pos = close_paren + 1;
                    continue;
                }
            }
        }

        // Check for inline code: `code`
        if (description[pos] == '`')
        {
            auto close_backtick = description.find('`', pos + 1);
            if (close_backtick != std::string::npos)
            {
                // Flush current text
                if (!current_text.empty())
                {
                    segments.push_back({DescriptionSegmentType::kText, current_text, ""});
                    current_text.clear();
                }

                auto code_text = description.substr(pos + 1, close_backtick - pos - 1);
                segments.push_back({DescriptionSegmentType::kCode, code_text, ""});
                pos = close_backtick + 1;
                continue;
            }
        }

        // Check for setting reference: {setting.id}
        if (description[pos] == '{')
        {
            auto close_brace = description.find('}', pos + 1);
            if (close_brace != std::string::npos)
            {
                auto ref_content = description.substr(pos + 1, close_brace - pos - 1);
                // Validate it looks like a setting ID (contains a dot)
                if (ref_content.find('.') != std::string::npos)
                {
                    // Flush current text
                    if (!current_text.empty())
                    {
                        segments.push_back({DescriptionSegmentType::kText, current_text, ""});
                        current_text.clear();
                    }

                    segments.push_back(
                        {DescriptionSegmentType::kSettingRef, ref_content, ref_content});
                    pos = close_brace + 1;
                    continue;
                }
            }
        }

        current_text += description[pos];
        ++pos;
    }

    // Flush remaining text
    if (!current_text.empty())
    {
        segments.push_back({DescriptionSegmentType::kText, current_text, ""});
    }

    return segments;
}

auto RichDescriptionRenderer::plain_text(const std::string& description) -> std::string
{
    auto segments = parse(description);
    std::string result;
    for (const auto& segment : segments)
    {
        result += segment.text;
    }
    return result;
}

} // namespace markamp::ui
