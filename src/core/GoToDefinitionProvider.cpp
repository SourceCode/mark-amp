#include "GoToDefinitionProvider.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

auto GoToDefinitionProvider::provide_definition(const std::string& content,
                                                int line,
                                                int character,
                                                const std::string& document_uri) const
    -> std::optional<LocationLink>
{
    const auto line_text = get_line_text(content, line);
    if (line_text.empty() || character < 0 || character > static_cast<int>(line_text.size()))
    {
        return std::nullopt;
    }

    // Priority: wiki-link > footnote > heading ref > markdown link
    auto wiki = resolve_wiki_link(line_text, character, line);
    if (wiki)
    {
        return wiki;
    }

    auto footnote = resolve_footnote_ref(line_text, character, line, content);
    if (footnote)
    {
        return footnote;
    }

    auto heading = resolve_heading_ref(line_text, character, line, content);
    if (heading)
    {
        return heading;
    }

    auto link = resolve_link(line_text, character, line, document_uri);
    if (link)
    {
        return link;
    }

    return std::nullopt;
}

void GoToDefinitionProvider::set_workspace_root(const std::string& root)
{
    workspace_root_ = root;
}

auto GoToDefinitionProvider::resolve_wiki_link(const std::string& line_text,
                                               int character,
                                               int line) const -> std::optional<LocationLink>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    auto open_pos = line_text.rfind("[[", char_pos);
    if (open_pos == std::string::npos)
    {
        return std::nullopt;
    }

    auto close_pos = line_text.find("]]", open_pos + 2);
    if (close_pos == std::string::npos || char_pos > close_pos + 1)
    {
        return std::nullopt;
    }

    auto target = line_text.substr(open_pos + 2, close_pos - open_pos - 2);

    // Strip display text after |
    auto pipe_pos = target.find('|');
    if (pipe_pos != std::string::npos)
    {
        target = target.substr(0, pipe_pos);
    }

    // Handle heading anchor after #
    std::string anchor;
    auto hash_pos = target.find('#');
    if (hash_pos != std::string::npos)
    {
        anchor = target.substr(hash_pos + 1);
        target = target.substr(0, hash_pos);
    }

    LocationLink result;
    result.origin_start_line = line;
    result.origin_start_char = static_cast<int>(open_pos);
    result.origin_end_line = line;
    result.origin_end_char = static_cast<int>(close_pos + 2);

    // Resolve target path
    if (!target.empty())
    {
        if (!workspace_root_.empty())
        {
            result.target_uri = workspace_root_ + "/" + target + ".md";
        }
        else
        {
            result.target_uri = target + ".md";
        }
    }

    result.target_start_line = 0;
    result.target_start_char = 0;
    result.target_end_line = 0;
    result.target_end_char = 0;

    return result;
}

auto GoToDefinitionProvider::resolve_heading_ref(const std::string& line_text,
                                                 int character,
                                                 int line,
                                                 const std::string& content)
    -> std::optional<LocationLink>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Look for #heading-ref in links: [text](#heading-ref)
    auto hash_pos = line_text.rfind('#', char_pos);
    if (hash_pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Must be within a link (preceded by ( or [)
    bool in_link = false;
    for (auto idx = hash_pos; idx > 0; --idx)
    {
        if (line_text[idx - 1] == '(')
        {
            in_link = true;
            break;
        }
        if (line_text[idx - 1] == ' ' || line_text[idx - 1] == '\t')
        {
            break;
        }
    }

    if (!in_link)
    {
        return std::nullopt;
    }

    // Extract heading slug until ) or space
    auto slug_start = hash_pos + 1;
    auto slug_end = slug_start;
    while (slug_end < line_text.size() && line_text[slug_end] != ')' &&
           line_text[slug_end] != ' ' && line_text[slug_end] != '"')
    {
        ++slug_end;
    }

    if (slug_end <= slug_start || char_pos > slug_end)
    {
        return std::nullopt;
    }

    auto heading_slug = line_text.substr(slug_start, slug_end - slug_start);
    auto target_line = find_heading_line(content, heading_slug);

    if (target_line < 0)
    {
        return std::nullopt;
    }

    LocationLink result;
    result.origin_start_line = line;
    result.origin_start_char = static_cast<int>(hash_pos);
    result.origin_end_line = line;
    result.origin_end_char = static_cast<int>(slug_end);
    result.target_start_line = target_line;
    result.target_start_char = 0;
    result.target_end_line = target_line;
    result.target_end_char = 0;

    return result;
}

auto GoToDefinitionProvider::resolve_footnote_ref(const std::string& line_text,
                                                  int character,
                                                  int line,
                                                  const std::string& content)
    -> std::optional<LocationLink>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    auto open_pos = line_text.rfind("[^", char_pos);
    if (open_pos == std::string::npos)
    {
        return std::nullopt;
    }

    auto close_pos = line_text.find(']', open_pos + 2);
    if (close_pos == std::string::npos || char_pos > close_pos)
    {
        return std::nullopt;
    }

    auto footnote_id = line_text.substr(open_pos + 2, close_pos - open_pos - 2);

    // Don't resolve definitions — only references
    if (close_pos + 1 < line_text.size() && line_text[close_pos + 1] == ':')
    {
        return std::nullopt;
    }

    auto target_line = find_footnote_line(content, footnote_id);
    if (target_line < 0)
    {
        return std::nullopt;
    }

    LocationLink result;
    result.origin_start_line = line;
    result.origin_start_char = static_cast<int>(open_pos);
    result.origin_end_line = line;
    result.origin_end_char = static_cast<int>(close_pos + 1);
    result.target_start_line = target_line;
    result.target_start_char = 0;
    result.target_end_line = target_line;
    auto def_line = get_line_text(content, target_line);
    result.target_end_char = static_cast<int>(def_line.size());

    return result;
}

auto GoToDefinitionProvider::resolve_link(const std::string& line_text,
                                          int character,
                                          int line,
                                          const std::string& document_uri) const
    -> std::optional<LocationLink>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    auto bracket_pos = line_text.rfind('[', char_pos);
    if (bracket_pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Skip images and wiki-links
    if (bracket_pos > 0 && (line_text[bracket_pos - 1] == '!' || line_text[bracket_pos - 1] == '['))
    {
        return std::nullopt;
    }

    auto close_bracket = line_text.find(']', bracket_pos + 1);
    if (close_bracket == std::string::npos || close_bracket + 1 >= line_text.size() ||
        line_text[close_bracket + 1] != '(')
    {
        return std::nullopt;
    }

    auto close_paren = line_text.find(')', close_bracket + 2);
    if (close_paren == std::string::npos || char_pos > close_paren)
    {
        return std::nullopt;
    }

    auto url = line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);

    // Strip title
    auto title_start = url.find('"');
    if (title_start != std::string::npos)
    {
        url = url.substr(0, title_start);
        while (!url.empty() && (url.back() == ' ' || url.back() == '\t'))
        {
            url.pop_back();
        }
    }

    // Only resolve relative file paths and heading refs
    if (url.empty() || url.find("://") != std::string::npos)
    {
        return std::nullopt; // External URL — don't resolve
    }

    LocationLink result;
    result.origin_start_line = line;
    result.origin_start_char = static_cast<int>(bracket_pos);
    result.origin_end_line = line;
    result.origin_end_char = static_cast<int>(close_paren + 1);

    // Resolve relative to workspace root or document
    if (!workspace_root_.empty() && url[0] != '/')
    {
        result.target_uri = workspace_root_ + "/" + url;
    }
    else if (!document_uri.empty() && url[0] != '/')
    {
        auto last_slash = document_uri.rfind('/');
        if (last_slash != std::string::npos)
        {
            result.target_uri = document_uri.substr(0, last_slash + 1) + url;
        }
        else
        {
            result.target_uri = url;
        }
    }
    else
    {
        result.target_uri = url;
    }

    result.target_start_line = 0;
    result.target_start_char = 0;
    result.target_end_line = 0;
    result.target_end_char = 0;

    return result;
}

auto GoToDefinitionProvider::get_line_text(const std::string& content, int line) -> std::string
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

auto GoToDefinitionProvider::find_heading_line(const std::string& content,
                                               const std::string& heading_slug) -> int
{
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line))
    {
        if (!current_line.empty() && current_line[0] == '#')
        {
            int level = 0;
            while (level < static_cast<int>(current_line.size()) &&
                   current_line[static_cast<std::string::size_type>(level)] == '#')
            {
                ++level;
            }
            if (level <= 6 && level < static_cast<int>(current_line.size()) &&
                current_line[static_cast<std::string::size_type>(level)] == ' ')
            {
                auto heading_text =
                    current_line.substr(static_cast<std::string::size_type>(level) + 1);
                if (slugify(heading_text) == heading_slug)
                {
                    return line_num;
                }
            }
        }
        ++line_num;
    }

    return -1;
}

auto GoToDefinitionProvider::find_footnote_line(const std::string& content,
                                                const std::string& footnote_id) -> int
{
    auto marker = "[^" + footnote_id + "]:";
    std::istringstream stream(content);
    std::string current_line;
    int line_num = 0;

    while (std::getline(stream, current_line))
    {
        if (current_line.find(marker) == 0)
        {
            return line_num;
        }
        ++line_num;
    }

    return -1;
}

auto GoToDefinitionProvider::slugify(const std::string& heading) -> std::string
{
    std::string result;
    result.reserve(heading.size());

    for (char ch : heading)
    {
        if (ch == ' ' || ch == '\t')
        {
            result += '-';
        }
        else if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_')
        {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        // Skip other characters (punctuation, etc.)
    }

    // Remove trailing hyphens
    while (!result.empty() && result.back() == '-')
    {
        result.pop_back();
    }

    return result;
}

} // namespace markamp::core
