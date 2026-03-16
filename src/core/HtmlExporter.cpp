// ============================================================================
// File: src/core/HtmlExporter.cpp
// Phase 39: Multi-Format Export — HtmlExporter implementation
// ============================================================================

#include "HtmlExporter.h"

#include <fstream>
#include <sstream>

namespace markamp::core
{

auto HtmlExporter::descriptor() const -> ExportFormatDescriptor
{
    return {.format = ExportFormat::HTML,
            .name = "HTML",
            .extension = ".html",
            .mime_type = "text/html",
            .description = "Standalone HTML document with embedded CSS",
            .requires_pandoc = false,
            .supports_assets = true,
            .supports_toc = true};
}

auto HtmlExporter::export_content(const std::string& markdown_source,
                                  const ExportOptions& options) const
    -> std::expected<std::string, std::string>
{
    auto body = md_to_html(markdown_source);

    // Load custom CSS or use default.
    std::string css = default_css();
    if (!options.css_path.empty())
    {
        std::ifstream css_file(options.css_path);
        if (css_file)
        {
            std::ostringstream ss;
            ss << css_file.rdbuf();
            css = ss.str();
        }
    }

    std::ostringstream html;
    html << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
         << "  <meta charset=\"UTF-8\">\n"
         << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
         << "  <title>Export</title>\n"
         << "  <style>\n"
         << css << "\n  </style>\n"
         << "</head>\n<body>\n"
         << "<article class=\"markdown-body\">\n"
         << body << "\n</article>\n</body>\n</html>\n";

    return html.str();
}

auto HtmlExporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto HtmlExporter::md_to_html(const std::string& markdown) -> std::string
{
    // Basic Markdown-to-HTML converter for common elements.
    std::istringstream stream(markdown);
    std::string line;
    std::ostringstream html;
    bool in_code = false;
    bool in_list = false;

    while (std::getline(stream, line))
    {
        // Code blocks.
        if (line.starts_with("```"))
        {
            if (in_code)
            {
                html << "</code></pre>\n";
                in_code = false;
            }
            else
            {
                auto lang = line.substr(3);
                html << "<pre><code class=\"language-" << lang << "\">";
                in_code = true;
            }
            continue;
        }
        if (in_code)
        {
            // Escape HTML in code.
            for (char c : line)
            {
                switch (c)
                {
                    case '<':
                        html << "&lt;";
                        break;
                    case '>':
                        html << "&gt;";
                        break;
                    case '&':
                        html << "&amp;";
                        break;
                    default:
                        html << c;
                }
            }
            html << "\n";
            continue;
        }

        // Headings.
        if (line.starts_with("#"))
        {
            int level = 0;
            while (level < static_cast<int>(line.size()) && line[level] == '#')
                ++level;
            if (level >= 1 && level <= 6)
            {
                auto text = line.substr(static_cast<size_t>(level));
                if (!text.empty() && text[0] == ' ')
                    text = text.substr(1);
                html << "<h" << level << ">" << text << "</h" << level << ">\n";
                continue;
            }
        }

        // Unordered lists.
        if (line.starts_with("- ") || line.starts_with("* "))
        {
            if (!in_list)
            {
                html << "<ul>\n";
                in_list = true;
            }
            html << "  <li>" << line.substr(2) << "</li>\n";
            continue;
        }
        if (in_list && (line.empty() || (!line.starts_with("- ") && !line.starts_with("* "))))
        {
            html << "</ul>\n";
            in_list = false;
        }

        // Horizontal rule.
        if (line == "---" || line == "***" || line == "___")
        {
            html << "<hr>\n";
            continue;
        }

        // (#35) Ordered lists.
        if (line.size() >= 3 && std::isdigit(static_cast<unsigned char>(line[0])))
        {
            auto dot_pos = line.find(". ");
            if (dot_pos != std::string::npos && dot_pos <= 3)
            {
                if (!in_list)
                {
                    html << "<ol>\n";
                    in_list = true;
                }
                html << "  <li>" << line.substr(dot_pos + 2) << "</li>\n";
                continue;
            }
        }

        // Blockquote.
        if (line.starts_with("> "))
        {
            html << "<blockquote>" << line.substr(2) << "</blockquote>\n";
            continue;
        }

        // Empty line = paragraph break.
        if (line.empty())
        {
            html << "\n";
            continue;
        }

        // Inline formatting.
        std::string processed;
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == '`')
            {
                auto end = line.find('`', i + 1);
                if (end != std::string::npos)
                {
                    processed += "<code>" + line.substr(i + 1, end - i - 1) + "</code>";
                    i = end;
                    continue;
                }
            }
            if (line[i] == '*' && i + 1 < line.size() && line[i + 1] == '*')
            {
                auto end = line.find("**", i + 2);
                if (end != std::string::npos)
                {
                    processed += "<strong>" + line.substr(i + 2, end - i - 2) + "</strong>";
                    i = end + 1;
                    continue;
                }
            }
            if (line[i] == '*')
            {
                auto end = line.find('*', i + 1);
                if (end != std::string::npos)
                {
                    processed += "<em>" + line.substr(i + 1, end - i - 1) + "</em>";
                    i = end;
                    continue;
                }
            }
            // (#34) Markdown links: [text](url)
            if (line[i] == '[')
            {
                auto bracket_end = line.find(']', i + 1);
                if (bracket_end != std::string::npos && bracket_end + 1 < line.size()
                    && line[bracket_end + 1] == '(')
                {
                    auto paren_end = line.find(')', bracket_end + 2);
                    if (paren_end != std::string::npos)
                    {
                        // Check if it's an image ![alt](url).
                        if (i > 0 && line[i - 1] == '!')
                        {
                            // Remove the '!' we already appended.
                            if (!processed.empty() && processed.back() == '!')
                            {
                                processed.pop_back();
                            }
                            auto alt = line.substr(i + 1, bracket_end - i - 1);
                            auto url = line.substr(bracket_end + 2, paren_end - bracket_end - 2);
                            processed += "<img src=\"" + url + "\" alt=\"" + alt + "\">";
                        }
                        else
                        {
                            auto text = line.substr(i + 1, bracket_end - i - 1);
                            auto url = line.substr(bracket_end + 2, paren_end - bracket_end - 2);
                            processed += "<a href=\"" + url + "\">" + text + "</a>";
                        }
                        i = paren_end;
                        continue;
                    }
                }
            }
            processed += line[i];
        }

        html << "<p>" << processed << "</p>\n";
    }

    if (in_list)
        html << "</ul>\n";
    if (in_code)
        html << "</code></pre>\n";

    return html.str();
}

auto HtmlExporter::default_css() -> std::string
{
    return R"(
    body { max-width: 800px; margin: 0 auto; padding: 2rem; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; line-height: 1.6; color: #333; }
    h1, h2, h3, h4, h5, h6 { margin-top: 1.5em; margin-bottom: 0.5em; }
    h1 { font-size: 2em; border-bottom: 1px solid #eee; padding-bottom: 0.3em; }
    h2 { font-size: 1.5em; border-bottom: 1px solid #eee; padding-bottom: 0.3em; }
    pre { background: #f6f8fa; padding: 1rem; border-radius: 6px; overflow-x: auto; }
    code { background: #f0f0f0; padding: 0.2em 0.4em; border-radius: 3px; font-size: 0.9em; }
    pre code { background: none; padding: 0; }
    blockquote { border-left: 3px solid #ddd; margin-left: 0; padding-left: 1em; color: #666; }
    img { max-width: 100%; }
    hr { border: none; border-top: 1px solid #eee; margin: 2em 0; }
    )";
}

} // namespace markamp::core
