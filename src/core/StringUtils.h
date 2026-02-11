#pragma once

#include <string>
#include <string_view>

namespace markamp::core
{

/// Shared HTML escape utility — replaces &, <, >, ", ' with HTML entities.
/// Used by HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer, SyntaxHighlighter.
[[nodiscard]] inline auto escape_html(std::string_view text) -> std::string
{
    std::string result;
    result.reserve(text.size());
    for (const char character : text)
    {
        switch (character)
        {
            case '&':
                result += "&amp;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&#39;";
                break;
            default:
                result += character;
                break;
        }
    }
    return result;
}

} // namespace markamp::core
