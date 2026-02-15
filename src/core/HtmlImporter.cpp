// ============================================================================
// File: src/core/HtmlImporter.cpp
// Phase 40: Import — HtmlImporter implementation
// ============================================================================

#include "HtmlImporter.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace markamp::core
{

auto HtmlImporter::descriptor() const -> ImportFormatDescriptor
{
    return {.format = ImportFormat::HTML,
            .name = "HTML",
            .extension = ".html",
            .all_extensions = {".html", ".htm", ".xhtml"},
            .description = "HTML documents",
            .requires_pandoc = false};
}

auto HtmlImporter::import_file(const std::filesystem::path& source_path,
                               const ImportOptions& /*options*/) const
    -> std::expected<std::string, std::string>
{
    std::ifstream file(source_path);
    if (!file)
        return std::unexpected("Cannot open: " + source_path.string());

    std::ostringstream ss;
    ss << file.rdbuf();
    return html_to_markdown(ss.str());
}

auto HtmlImporter::check_dependencies() const -> std::expected<void, std::string>
{
    return {};
}

auto HtmlImporter::can_import(const std::filesystem::path& file_path) const -> bool
{
    auto ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".html" || ext == ".htm" || ext == ".xhtml";
}

auto HtmlImporter::html_to_markdown(const std::string& html) -> std::string
{
    // Basic HTML→Markdown: handle common tags.
    std::string result;
    result.reserve(html.size());

    size_t i = 0;
    while (i < html.size())
    {
        if (html[i] == '<')
        {
            auto close = html.find('>', i);
            if (close == std::string::npos)
            {
                result += html[i++];
                continue;
            }

            auto tag_content = html.substr(i + 1, close - i - 1);
            auto tag = tag_content;
            // Extract just the tag name.
            auto space_pos = tag.find(' ');
            if (space_pos != std::string::npos)
                tag = tag.substr(0, space_pos);
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

            // Map HTML tags to Markdown.
            if (tag == "h1")
                result += "# ";
            else if (tag == "h2")
                result += "## ";
            else if (tag == "h3")
                result += "### ";
            else if (tag == "h4")
                result += "#### ";
            else if (tag == "h5")
                result += "##### ";
            else if (tag == "h6")
                result += "###### ";
            else if (tag == "/h1" || tag == "/h2" || tag == "/h3" || tag == "/h4" || tag == "/h5" ||
                     tag == "/h6")
                result += "\n\n";
            else if (tag == "p")
                result += "";
            else if (tag == "/p")
                result += "\n\n";
            else if (tag == "br" || tag == "br/")
                result += "\n";
            else if (tag == "strong" || tag == "b")
                result += "**";
            else if (tag == "/strong" || tag == "/b")
                result += "**";
            else if (tag == "em" || tag == "i")
                result += "*";
            else if (tag == "/em" || tag == "/i")
                result += "*";
            else if (tag == "code")
                result += "`";
            else if (tag == "/code")
                result += "`";
            else if (tag == "li")
                result += "- ";
            else if (tag == "/li")
                result += "\n";
            else if (tag == "blockquote")
                result += "> ";
            else if (tag == "/blockquote")
                result += "\n";
            else if (tag == "hr")
                result += "\n---\n";
            else if (tag == "pre")
                result += "\n```\n";
            else if (tag == "/pre")
                result += "\n```\n";
            // Skip script, style, head etc.
            else if (tag == "script" || tag == "style")
            {
                auto end_tag = "</" + tag + ">";
                auto end_pos = html.find(end_tag, close);
                if (end_pos != std::string::npos)
                {
                    i = end_pos + end_tag.size();
                    continue;
                }
            }

            i = close + 1;
        }
        else
        {
            result += html[i++];
        }
    }

    return result;
}

} // namespace markamp::core
