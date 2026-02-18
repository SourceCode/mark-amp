#include "HoverProvider.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

auto HoverProvider::provide_hover(const std::string& content, int line, int character) const
    -> std::optional<HoverContent>
{
    const auto line_text = get_line_text(content, line);
    if (line_text.empty() || character < 0 || character > static_cast<int>(line_text.size()))
    {
        return std::nullopt;
    }

    // Priority order: image > link > wiki-link > footnote > heading > code span > emoji

    if (show_image_info_)
    {
        auto image_hover = hover_image(line_text, character, line);
        if (image_hover)
        {
            return image_hover;
        }
    }

    if (resolve_links_)
    {
        auto link_hover = hover_link(line_text, character, line);
        if (link_hover)
        {
            return link_hover;
        }
    }

    auto wiki_hover = hover_wiki_link(line_text, character, line);
    if (wiki_hover)
    {
        return wiki_hover;
    }

    auto footnote_hover = hover_footnote(line_text, character, line, content);
    if (footnote_hover)
    {
        return footnote_hover;
    }

    auto heading_hover = hover_heading(line_text, line, content);
    if (heading_hover)
    {
        return heading_hover;
    }

    auto code_hover = hover_code_span(line_text, character, line);
    if (code_hover)
    {
        return code_hover;
    }

    auto emoji_hover = hover_emoji(line_text, character, line);
    if (emoji_hover)
    {
        return emoji_hover;
    }

    return std::nullopt;
}

void HoverProvider::set_resolve_links(bool enabled)
{
    resolve_links_ = enabled;
}

void HoverProvider::set_show_image_info(bool enabled)
{
    show_image_info_ = enabled;
}

auto HoverProvider::hover_link(const std::string& line_text, int character, int line)
    -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find [text](url) around cursor — look for [ before cursor
    auto bracket_pos = line_text.rfind('[', char_pos);
    if (bracket_pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Make sure it's not an image (preceded by !)
    if (bracket_pos > 0 && line_text[bracket_pos - 1] == '!')
    {
        return std::nullopt;
    }

    // Make sure it's not a wiki-link
    if (bracket_pos > 0 && line_text[bracket_pos - 1] == '[')
    {
        return std::nullopt;
    }

    auto close_bracket = line_text.find(']', bracket_pos + 1);
    if (close_bracket == std::string::npos)
    {
        return std::nullopt;
    }

    // Check for (url) after ]
    if (close_bracket + 1 >= line_text.size() || line_text[close_bracket + 1] != '(')
    {
        return std::nullopt;
    }

    auto close_paren = line_text.find(')', close_bracket + 2);
    if (close_paren == std::string::npos)
    {
        return std::nullopt;
    }

    // Verify cursor is within the link bounds
    if (char_pos < bracket_pos || char_pos > close_paren)
    {
        return std::nullopt;
    }

    auto link_text = line_text.substr(bracket_pos + 1, close_bracket - bracket_pos - 1);
    auto url = line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);

    // Strip optional title from URL
    std::string title;
    auto title_start = url.find('"');
    if (title_start != std::string::npos)
    {
        auto title_end = url.find('"', title_start + 1);
        if (title_end != std::string::npos)
        {
            title = url.substr(title_start + 1, title_end - title_start - 1);
        }
        // Remove title from URL
        url = url.substr(0, title_start);
        // Trim trailing whitespace from URL
        while (!url.empty() && (url.back() == ' ' || url.back() == '\t'))
        {
            url.pop_back();
        }
    }

    HoverContent result;
    result.start_line = line;
    result.start_char = static_cast<int>(bracket_pos);
    result.end_line = line;
    result.end_char = static_cast<int>(close_paren + 1);

    result.markdown = "**Link:** " + link_text + "\n\n";
    result.markdown += "**URL:** `" + url + "`\n";
    if (!title.empty())
    {
        result.markdown += "\n**Title:** " + title + "\n";
    }

    return result;
}

auto HoverProvider::hover_wiki_link(const std::string& line_text, int character, int line)
    -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find [[ before cursor
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

    // Handle display text: [[target|display]]
    std::string display;
    auto pipe_pos = target.find('|');
    if (pipe_pos != std::string::npos)
    {
        display = target.substr(pipe_pos + 1);
        target = target.substr(0, pipe_pos);
    }

    // Handle heading anchor: [[target#heading]]
    std::string anchor;
    auto hash_pos = target.find('#');
    if (hash_pos != std::string::npos)
    {
        anchor = target.substr(hash_pos + 1);
        target = target.substr(0, hash_pos);
    }

    HoverContent result;
    result.start_line = line;
    result.start_char = static_cast<int>(open_pos);
    result.end_line = line;
    result.end_char = static_cast<int>(close_pos + 2);

    result.markdown = "**Wiki Link**\n\n";
    result.markdown += "**Target:** `" + target + "`\n";
    if (!anchor.empty())
    {
        result.markdown += "\n**Heading:** `#" + anchor + "`\n";
    }
    if (!display.empty())
    {
        result.markdown += "\n**Display:** " + display + "\n";
    }

    return result;
}

auto HoverProvider::hover_heading(const std::string& line_text,
                                  int line,
                                  const std::string& content) -> std::optional<HoverContent>
{
    if (line_text.empty() || line_text[0] != '#')
    {
        return std::nullopt;
    }

    int level = 0;
    while (level < static_cast<int>(line_text.size()) &&
           line_text[static_cast<std::string::size_type>(level)] == '#')
    {
        ++level;
    }

    if (level > 6 || level >= static_cast<int>(line_text.size()) ||
        line_text[static_cast<std::string::size_type>(level)] != ' ')
    {
        return std::nullopt;
    }

    auto heading_text = line_text.substr(static_cast<std::string::size_type>(level) + 1);
    auto word_count = count_section_words(content, line, level);

    HoverContent result;
    result.start_line = line;
    result.start_char = 0;
    result.end_line = line;
    result.end_char = static_cast<int>(line_text.size());

    result.markdown = "**Heading Level " + std::to_string(level) + "**\n\n";
    result.markdown += "`" + heading_text + "`\n\n";
    result.markdown += "Section word count: **" + std::to_string(word_count) + "**\n";

    return result;
}

auto HoverProvider::hover_image(const std::string& line_text, int character, int line)
    -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find ![ before cursor
    auto excl_pos = line_text.rfind("![", char_pos);
    if (excl_pos == std::string::npos)
    {
        return std::nullopt;
    }

    auto close_bracket = line_text.find(']', excl_pos + 2);
    if (close_bracket == std::string::npos)
    {
        return std::nullopt;
    }

    if (close_bracket + 1 >= line_text.size() || line_text[close_bracket + 1] != '(')
    {
        return std::nullopt;
    }

    auto close_paren = line_text.find(')', close_bracket + 2);
    if (close_paren == std::string::npos || char_pos > close_paren)
    {
        return std::nullopt;
    }

    auto alt_text = line_text.substr(excl_pos + 2, close_bracket - excl_pos - 2);
    auto url = line_text.substr(close_bracket + 2, close_paren - close_bracket - 2);

    // Strip title
    std::string title;
    auto title_start = url.find('"');
    if (title_start != std::string::npos)
    {
        auto title_end = url.find('"', title_start + 1);
        if (title_end != std::string::npos)
        {
            title = url.substr(title_start + 1, title_end - title_start - 1);
        }
        url = url.substr(0, title_start);
        while (!url.empty() && (url.back() == ' ' || url.back() == '\t'))
        {
            url.pop_back();
        }
    }

    HoverContent result;
    result.start_line = line;
    result.start_char = static_cast<int>(excl_pos);
    result.end_line = line;
    result.end_char = static_cast<int>(close_paren + 1);

    result.markdown = "**Image**\n\n";
    result.markdown += "**Source:** `" + url + "`\n";
    if (!alt_text.empty())
    {
        result.markdown += "\n**Alt text:** " + alt_text + "\n";
    }
    if (!title.empty())
    {
        result.markdown += "\n**Title:** " + title + "\n";
    }

    return result;
}

auto HoverProvider::hover_footnote(const std::string& line_text,
                                   int character,
                                   int line,
                                   const std::string& content) -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find [^ before cursor
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

    // This is a reference, not a definition (definition has ]: after)
    auto footnote_id = line_text.substr(open_pos + 2, close_pos - open_pos - 2);
    bool is_definition = (close_pos + 1 < line_text.size() && line_text[close_pos + 1] == ':');

    HoverContent result;
    result.start_line = line;
    result.start_char = static_cast<int>(open_pos);
    result.end_line = line;
    result.end_char = static_cast<int>(close_pos + 1);

    if (is_definition)
    {
        result.markdown = "**Footnote Definition**\n\n";
        result.markdown += "ID: `" + footnote_id + "`\n";
    }
    else
    {
        auto definition = find_footnote_definition(content, footnote_id);
        result.markdown = "**Footnote Reference** `[^" + footnote_id + "]`\n\n";
        if (!definition.empty())
        {
            result.markdown += "**Definition:**\n\n" + definition + "\n";
        }
        else
        {
            result.markdown += "*Definition not found*\n";
        }
    }

    return result;
}

auto HoverProvider::hover_code_span(const std::string& line_text, int character, int line)
    -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find ` before cursor
    auto open_pos = line_text.rfind('`', char_pos);
    if (open_pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Find closing `
    auto close_pos = line_text.find('`', open_pos + 1);
    if (close_pos == std::string::npos || close_pos <= open_pos || char_pos > close_pos)
    {
        return std::nullopt;
    }

    // Make sure cursor is between the backticks
    if (char_pos <= open_pos)
    {
        return std::nullopt;
    }

    auto code_text = line_text.substr(open_pos + 1, close_pos - open_pos - 1);

    HoverContent result;
    result.start_line = line;
    result.start_char = static_cast<int>(open_pos);
    result.end_line = line;
    result.end_char = static_cast<int>(close_pos + 1);

    result.markdown = "**Inline Code**\n\n`" + code_text + "`\n\n";
    result.markdown += "Length: " + std::to_string(code_text.size()) + " characters\n";

    return result;
}

auto HoverProvider::hover_emoji(const std::string& line_text, int character, int line)
    -> std::optional<HoverContent>
{
    auto char_pos = static_cast<std::string::size_type>(character);

    // Find : before cursor
    auto open_pos = line_text.rfind(':', char_pos);
    if (open_pos == std::string::npos)
    {
        return std::nullopt;
    }

    // Find closing :
    auto close_pos = line_text.find(':', open_pos + 1);
    if (close_pos == std::string::npos || close_pos <= open_pos || char_pos > close_pos)
    {
        return std::nullopt;
    }

    auto shortcode = line_text.substr(open_pos + 1, close_pos - open_pos - 1);

    // Validate: no spaces in shortcode
    if (shortcode.find(' ') != std::string::npos || shortcode.empty())
    {
        return std::nullopt;
    }

    // Look up emoji
    struct EmojiEntry
    {
        const char* shortcode;
        const char* emoji;
        const char* description;
    };
    static const EmojiEntry kEmojis[] = {
        {"smile", "😄", "Smiling Face with Open Mouth"},
        {"heart", "❤️", "Red Heart"},
        {"thumbsup", "👍", "Thumbs Up"},
        {"fire", "🔥", "Fire"},
        {"star", "⭐", "Star"},
        {"check", "✅", "Check Mark"},
        {"x", "❌", "Cross Mark"},
        {"warning", "⚠️", "Warning Sign"},
        {"rocket", "🚀", "Rocket"},
        {"sparkles", "✨", "Sparkles"},
        {"tada", "🎉", "Party Popper"},
        {"bulb", "💡", "Light Bulb"},
        {"memo", "📝", "Memo"},
        {"wave", "👋", "Waving Hand"},
        {"thinking", "🤔", "Thinking Face"},
        {"eyes", "👀", "Eyes"},
        {"zap", "⚡", "High Voltage"},
        {"bug", "🐛", "Bug"},
        {"wrench", "🔧", "Wrench"},
        {"gear", "⚙️", "Gear"},
    };

    for (const auto& entry : kEmojis)
    {
        if (shortcode == entry.shortcode)
        {
            HoverContent result;
            result.start_line = line;
            result.start_char = static_cast<int>(open_pos);
            result.end_line = line;
            result.end_char = static_cast<int>(close_pos + 1);
            result.markdown = std::string("**Emoji:** ") + entry.emoji + "\n\n";
            result.markdown += std::string("**Name:** ") + entry.description + "\n\n";
            result.markdown += std::string("**Shortcode:** `:") + entry.shortcode + ":`\n";
            return result;
        }
    }

    return std::nullopt;
}

auto HoverProvider::get_line_text(const std::string& content, int line) -> std::string
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

auto HoverProvider::count_section_words(const std::string& content,
                                        int heading_line,
                                        int heading_level) -> int
{
    std::istringstream stream(content);
    std::string current_line;
    int current = 0;
    int word_count = 0;
    bool in_section = false;

    while (std::getline(stream, current_line))
    {
        if (current == heading_line)
        {
            in_section = true;
            ++current;
            continue;
        }

        if (in_section)
        {
            // Stop at same or higher level heading
            if (!current_line.empty() && current_line[0] == '#')
            {
                int level = 0;
                while (level < static_cast<int>(current_line.size()) &&
                       current_line[static_cast<std::string::size_type>(level)] == '#')
                {
                    ++level;
                }
                if (level <= heading_level)
                {
                    break;
                }
            }

            // Count words in this line
            std::istringstream word_stream(current_line);
            std::string word;
            while (word_stream >> word)
            {
                ++word_count;
            }
        }
        ++current;
    }

    return word_count;
}

auto HoverProvider::find_footnote_definition(const std::string& content,
                                             const std::string& footnote_id) -> std::string
{
    auto marker = "[^" + footnote_id + "]:";
    auto pos = content.find(marker);
    if (pos == std::string::npos)
    {
        return "";
    }

    auto def_start = pos + marker.size();
    // Skip leading whitespace
    while (def_start < content.size() && (content[def_start] == ' ' || content[def_start] == '\t'))
    {
        ++def_start;
    }

    // Read until end of line
    auto def_end = content.find('\n', def_start);
    if (def_end == std::string::npos)
    {
        def_end = content.size();
    }

    return content.substr(def_start, def_end - def_start);
}

} // namespace markamp::core
