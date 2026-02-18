#include "CompletionProvider.h"

#include "OutlineService.h"
#include "SnippetEngine.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

CompletionProvider::CompletionProvider(SnippetEngine& snippet_engine,
                                       OutlineService& outline_service)
    : snippet_engine_(snippet_engine)
    , outline_service_(outline_service)
{
}

auto CompletionProvider::provide_completions(const std::string& content,
                                             int line,
                                             int character,
                                             const CompletionContext& context) const
    -> CompletionList
{
    CompletionList result;
    const auto line_text = get_line_text(content, line);
    const auto prefix = extract_prefix(content, line, character);

    // Wiki-link completion: [[prefix
    if (is_in_wiki_link(line_text, character) && is_source_enabled(CompletionSource::kLinks))
    {
        auto heading_items = get_heading_completions(prefix, content);
        for (auto& item : heading_items)
        {
            item.kind = CompletionKind::kLink;
            result.items.push_back(std::move(item));
        }
        result.is_incomplete = true;
        return result;
    }

    // Heading reference: #prefix
    if (is_heading_ref(prefix) && is_source_enabled(CompletionSource::kHeadings))
    {
        auto heading_prefix = prefix.substr(1); // Remove leading #
        auto items = get_heading_completions(heading_prefix, content);
        result.items = std::move(items);
        return result;
    }

    // Emoji shortcode: :prefix
    if (is_emoji_shortcode(line_text, character) && is_source_enabled(CompletionSource::kEmoji))
    {
        result.items = get_emoji_completions(prefix);
        return result;
    }

    // Trigger character completions
    if (context.trigger_kind == CompletionTriggerKind::kTriggerCharacter)
    {
        if (context.trigger_character == "/" &&
            is_source_enabled(CompletionSource::kMarkdownSyntax))
        {
            result.items = get_markdown_syntax_completions("");
            return result;
        }
    }

    // General prefix-based completions — aggregate all enabled sources
    if (!prefix.empty())
    {
        if (is_source_enabled(CompletionSource::kSnippets))
        {
            auto snippets = get_snippet_completions(prefix);
            for (auto& item : snippets)
            {
                result.items.push_back(std::move(item));
            }
        }

        if (is_source_enabled(CompletionSource::kHeadings))
        {
            auto headings = get_heading_completions(prefix, content);
            for (auto& item : headings)
            {
                result.items.push_back(std::move(item));
            }
        }

        if (is_source_enabled(CompletionSource::kMarkdownSyntax))
        {
            auto syntax = get_markdown_syntax_completions(prefix);
            for (auto& item : syntax)
            {
                result.items.push_back(std::move(item));
            }
        }
    }

    // Sort by score descending
    std::sort(result.items.begin(),
              result.items.end(),
              [](const CompletionItem& item_a, const CompletionItem& item_b)
              { return item_a.score > item_b.score; });

    // Trim to reasonable limit
    const auto max_total = static_cast<std::size_t>(max_items_per_source_ * 3);
    if (result.items.size() > max_total)
    {
        result.items.resize(max_total);
        result.is_incomplete = true;
    }

    return result;
}

auto CompletionProvider::provide_signature_help(const std::string& content,
                                                int line,
                                                int character) const -> SignatureHelpResult
{
    SignatureHelpResult result;
    const auto line_text = get_line_text(content, line);

    if (character < 0 || character > static_cast<int>(line_text.size()))
    {
        return result;
    }

    // Check for image syntax: ![alt](url "title")
    auto excl_pos = line_text.rfind("![", static_cast<std::string::size_type>(character));
    if (excl_pos != std::string::npos)
    {
        auto close_bracket = line_text.find(']', excl_pos + 2);
        if (close_bracket != std::string::npos && close_bracket < line_text.size() - 1 &&
            line_text[close_bracket + 1] == '(')
        {
            SignatureInfo sig;
            sig.label = "![alt text](url \"title\")";
            sig.documentation = "Insert an image with alt text, URL, and optional title.";
            sig.parameters = {
                {"alt text", "Alternative text displayed when image cannot be loaded"},
                {"url", "URL or relative path to the image file"},
                {"\"title\"", "Optional title shown on hover (in quotes)"}};

            auto char_pos = static_cast<std::string::size_type>(character);
            if (char_pos <= close_bracket)
            {
                sig.active_parameter = 0; // Inside alt text
            }
            else
            {
                auto paren_pos = close_bracket + 1;
                auto quote_pos = line_text.find('"', paren_pos + 1);
                if (quote_pos != std::string::npos && char_pos >= quote_pos)
                {
                    sig.active_parameter = 2; // Inside title
                }
                else
                {
                    sig.active_parameter = 1; // Inside URL
                }
            }

            result.signatures.push_back(std::move(sig));
            result.has_result = true;
            return result;
        }
    }

    // Check for link syntax: [text](url "title")
    auto bracket_pos = line_text.rfind('[', static_cast<std::string::size_type>(character));
    if (bracket_pos != std::string::npos && (bracket_pos == 0 || line_text[bracket_pos - 1] != '!'))
    {
        auto close_bracket = line_text.find(']', bracket_pos + 1);
        if (close_bracket != std::string::npos && close_bracket < line_text.size() - 1 &&
            line_text[close_bracket + 1] == '(')
        {
            SignatureInfo sig;
            sig.label = "[link text](url \"title\")";
            sig.documentation = "Insert a hyperlink with display text, URL, and optional title.";
            sig.parameters = {{"link text", "Text displayed for the link"},
                              {"url", "URL or relative path to the target"},
                              {"\"title\"", "Optional title shown on hover (in quotes)"}};

            auto char_pos = static_cast<std::string::size_type>(character);
            if (char_pos <= close_bracket)
            {
                sig.active_parameter = 0;
            }
            else
            {
                auto paren_pos = close_bracket + 1;
                auto quote_pos = line_text.find('"', paren_pos + 1);
                if (quote_pos != std::string::npos && char_pos >= quote_pos)
                {
                    sig.active_parameter = 2;
                }
                else
                {
                    sig.active_parameter = 1;
                }
            }

            result.signatures.push_back(std::move(sig));
            result.has_result = true;
        }
    }

    return result;
}

void CompletionProvider::set_enabled_sources(CompletionSource sources)
{
    enabled_sources_ = sources;
}

auto CompletionProvider::enabled_sources() const -> CompletionSource
{
    return enabled_sources_;
}

void CompletionProvider::set_max_items_per_source(int max_items)
{
    max_items_per_source_ = max_items;
}

auto CompletionProvider::extract_prefix(const std::string& content, int line, int character)
    -> std::string
{
    auto line_text = get_line_text(content, line);
    if (character <= 0 || character > static_cast<int>(line_text.size()))
    {
        return "";
    }

    // Walk backward from cursor to find word start
    auto pos = static_cast<std::string::size_type>(character);
    while (pos > 0)
    {
        char prev = line_text[pos - 1];
        // Stop at whitespace or certain punctuation
        if (prev == ' ' || prev == '\t' || prev == '(' || prev == ')' || prev == ',' || prev == ';')
        {
            break;
        }
        --pos;
    }

    return line_text.substr(pos, static_cast<std::string::size_type>(character) - pos);
}

auto CompletionProvider::get_line_text(const std::string& content, int line) -> std::string
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

auto CompletionProvider::is_in_wiki_link(const std::string& line_text, int character) -> bool
{
    if (character < 2)
    {
        return false;
    }
    auto pos = static_cast<std::string::size_type>(character);
    // Look backward for [[ without a closing ]]
    auto open_pos = line_text.rfind("[[", pos);
    if (open_pos == std::string::npos)
    {
        return false;
    }
    auto close_pos = line_text.find("]]", open_pos + 2);
    return close_pos == std::string::npos || close_pos >= pos;
}

auto CompletionProvider::is_heading_ref(const std::string& prefix) -> bool
{
    return !prefix.empty() && prefix[0] == '#' && prefix.size() > 1;
}

auto CompletionProvider::is_emoji_shortcode(const std::string& line_text, int character) -> bool
{
    if (character < 1)
    {
        return false;
    }
    auto pos = static_cast<std::string::size_type>(character);
    // Look backward for : without whitespace
    for (auto idx = pos; idx > 0; --idx)
    {
        if (line_text[idx - 1] == ':')
        {
            // Check that there's no space between : and cursor
            return true;
        }
        if (line_text[idx - 1] == ' ' || line_text[idx - 1] == '\t')
        {
            return false;
        }
    }
    return false;
}

auto CompletionProvider::is_source_enabled(CompletionSource source) const -> bool
{
    return (enabled_sources_ & source) != CompletionSource::kNone;
}

auto CompletionProvider::get_snippet_completions(const std::string& prefix) const
    -> std::vector<CompletionItem>
{
    std::vector<CompletionItem> items;
    auto snippets = snippet_engine_.get_for_prefix(prefix);

    int count = 0;
    for (const auto& snippet : snippets)
    {
        if (count >= max_items_per_source_)
        {
            break;
        }
        CompletionItem item;
        item.label = snippet.prefix;
        item.detail = snippet.name;
        item.documentation = snippet.description;
        item.insert_text = snippet.body;
        item.kind = CompletionKind::kSnippet;
        item.is_snippet = true;
        item.score = 90.0 - static_cast<double>(count); // High priority for snippets
        items.push_back(std::move(item));
        ++count;
    }

    return items;
}

auto CompletionProvider::get_heading_completions(const std::string& prefix,
                                                 const std::string& content) const
    -> std::vector<CompletionItem>
{
    std::vector<CompletionItem> items;

    // Parse headings from content directly (lightweight scan)
    std::istringstream stream(content);
    std::string line_text;
    int line_num = 0;
    int count = 0;

    while (std::getline(stream, line_text) && count < max_items_per_source_)
    {
        if (!line_text.empty() && line_text[0] == '#')
        {
            // Count heading level
            int level = 0;
            while (level < static_cast<int>(line_text.size()) &&
                   line_text[static_cast<std::string::size_type>(level)] == '#')
            {
                ++level;
            }
            if (level <= 6 && level < static_cast<int>(line_text.size()) &&
                line_text[static_cast<std::string::size_type>(level)] == ' ')
            {
                auto heading_text =
                    line_text.substr(static_cast<std::string::size_type>(level) + 1);

                // Filter by prefix (case-insensitive)
                if (prefix.empty() || heading_text.find(prefix) != std::string::npos)
                {
                    CompletionItem item;
                    item.label = heading_text;
                    item.detail =
                        "H" + std::to_string(level) + " — line " + std::to_string(line_num + 1);
                    item.insert_text = heading_text;
                    item.kind = CompletionKind::kHeading;
                    item.score = 70.0 - static_cast<double>(count);
                    items.push_back(std::move(item));
                    ++count;
                }
            }
        }
        ++line_num;
    }

    return items;
}

auto CompletionProvider::get_markdown_syntax_completions(const std::string& prefix)
    -> std::vector<CompletionItem>
{
    // Static list of common markdown syntax completions
    struct SyntaxTemplate
    {
        const char* label;
        const char* insert;
        const char* detail;
        bool is_snippet;
    };

    static const SyntaxTemplate kTemplates[] = {
        {"- [ ] ", "- [ ] $0", "Task list item", true},
        {"> [!NOTE]", "> [!NOTE]\n> $0", "Note callout", true},
        {"> [!TIP]", "> [!TIP]\n> $0", "Tip callout", true},
        {"> [!WARNING]", "> [!WARNING]\n> $0", "Warning callout", true},
        {"> [!IMPORTANT]", "> [!IMPORTANT]\n> $0", "Important callout", true},
        {"> [!CAUTION]", "> [!CAUTION]\n> $0", "Caution callout", true},
        {"```", "```${1:language}\n$0\n```", "Code fence", true},
        {"---", "---\n$0\n---", "YAML frontmatter", true},
        {"| ", "| ${1:Header 1} | ${2:Header 2} |\n| --- | --- |\n| $0 |  |", "Table", true},
        {"[^", "[^${1:id}]: $0", "Footnote definition", true},
        {"<details>",
         "<details>\n<summary>${1:Summary}</summary>\n\n$0\n\n</details>",
         "Collapsible section",
         true},
    };

    std::vector<CompletionItem> items;
    int count = 0;
    for (const auto& tmpl : kTemplates)
    {
        std::string label_str(tmpl.label);
        if (prefix.empty() || label_str.find(prefix) != std::string::npos)
        {
            CompletionItem item;
            item.label = label_str;
            item.insert_text = tmpl.insert;
            item.detail = tmpl.detail;
            item.kind = CompletionKind::kKeyword;
            item.is_snippet = tmpl.is_snippet;
            item.score = 50.0 - static_cast<double>(count);
            items.push_back(std::move(item));
            ++count;
        }
    }

    return items;
}

auto CompletionProvider::get_emoji_completions(const std::string& prefix)
    -> std::vector<CompletionItem>
{
    // Common emoji shortcodes — a representative subset
    struct EmojiEntry
    {
        const char* shortcode;
        const char* emoji;
    };

    static const EmojiEntry kEmojis[] = {
        {"smile", "😄"},    {"laughing", "😆"},   {"wink", "😉"}, {"heart", "❤️"},
        {"thumbsup", "👍"}, {"thumbsdown", "👎"}, {"clap", "👏"}, {"fire", "🔥"},
        {"star", "⭐"},     {"check", "✅"},      {"x", "❌"},    {"warning", "⚠️"},
        {"bulb", "💡"},     {"rocket", "🚀"},     {"eyes", "👀"}, {"memo", "📝"},
        {"link", "🔗"},     {"lock", "🔒"},       {"key", "🔑"},  {"bug", "🐛"},
        {"wrench", "🔧"},   {"sparkles", "✨"},   {"tada", "🎉"}, {"thinking", "🤔"},
        {"wave", "👋"},     {"100", "💯"},        {"zap", "⚡"},  {"globe", "🌍"},
        {"package", "📦"},  {"gear", "⚙️"},
    };

    std::vector<CompletionItem> items;
    int count = 0;
    for (const auto& entry : kEmojis)
    {
        std::string shortcode_str(entry.shortcode);
        if (prefix.empty() || shortcode_str.find(prefix) != std::string::npos)
        {
            CompletionItem item;
            item.label = std::string(":") + entry.shortcode + ":";
            item.detail = entry.emoji;
            item.insert_text = entry.emoji;
            item.kind = CompletionKind::kEmoji;
            item.score = 60.0 - static_cast<double>(count);
            items.push_back(std::move(item));
            ++count;
            if (count >= 15)
            {
                break;
            }
        }
    }

    return items;
}

} // namespace markamp::core
