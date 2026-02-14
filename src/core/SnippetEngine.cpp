#include "SnippetEngine.h"

#include "IPlugin.h" // For SnippetContribution

#include <algorithm>
#include <sstream>

namespace markamp::core
{

void SnippetEngine::register_snippet(Snippet snippet)
{
    const auto prefix = snippet.prefix;
    snippets_.push_back(std::move(snippet));
    prefix_index_[prefix] = snippets_.size() - 1;
}

auto SnippetEngine::register_from_json(const std::string& json_string) -> std::size_t
{
    // Lightweight JSON parsing for VS Code snippet format:
    // { "Name": { "prefix": "...", "body": ["line1", "line2"], "description": "..." } }
    //
    // This is a simplified parser that handles the common cases without requiring
    // a full JSON library dependency.

    std::size_t count = 0;
    std::size_t pos = 0;
    const auto len = json_string.size();

    // Helper: skip whitespace
    auto skip_ws = [&]()
    {
        while (pos < len && (json_string[pos] == ' ' || json_string[pos] == '\t' ||
                             json_string[pos] == '\n' || json_string[pos] == '\r'))
        {
            ++pos;
        }
    };

    // Helper: parse a quoted string
    auto parse_string = [&]() -> std::string
    {
        if (pos >= len || json_string[pos] != '"')
        {
            return {};
        }
        ++pos; // skip opening quote
        std::string result;
        while (pos < len && json_string[pos] != '"')
        {
            if (json_string[pos] == '\\' && pos + 1 < len)
            {
                ++pos;
                switch (json_string[pos])
                {
                    case 'n':
                        result += '\n';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    case '"':
                        result += '"';
                        break;
                    default:
                        result += '\\';
                        result += json_string[pos];
                        break;
                }
            }
            else
            {
                result += json_string[pos];
            }
            ++pos;
        }
        if (pos < len)
        {
            ++pos; // skip closing quote
        }
        return result;
    };

    // Helper: parse body (string or array of strings)
    auto parse_body = [&]() -> std::string
    {
        skip_ws();
        if (pos >= len)
        {
            return {};
        }

        if (json_string[pos] == '"')
        {
            return parse_string();
        }

        if (json_string[pos] == '[')
        {
            ++pos; // skip '['
            std::string body;
            bool first = true;
            skip_ws();
            while (pos < len && json_string[pos] != ']')
            {
                if (!first)
                {
                    body += '\n';
                }
                skip_ws();
                if (pos < len && json_string[pos] == ',')
                {
                    ++pos;
                    skip_ws();
                }
                if (pos < len && json_string[pos] == '"')
                {
                    body += parse_string();
                }
                first = false;
                skip_ws();
                if (pos < len && json_string[pos] == ',')
                {
                    ++pos;
                }
            }
            if (pos < len)
            {
                ++pos; // skip ']'
            }
            return body;
        }

        return {};
    };

    // Parse top-level object
    skip_ws();
    if (pos >= len || json_string[pos] != '{')
    {
        return 0;
    }
    ++pos; // skip '{'

    while (pos < len)
    {
        skip_ws();
        if (pos >= len || json_string[pos] == '}')
        {
            break;
        }

        if (json_string[pos] == ',')
        {
            ++pos;
            continue;
        }

        // Parse snippet name (key)
        auto name = parse_string();
        if (name.empty())
        {
            break;
        }

        skip_ws();
        if (pos >= len || json_string[pos] != ':')
        {
            break;
        }
        ++pos; // skip ':'

        // Parse snippet object
        skip_ws();
        if (pos >= len || json_string[pos] != '{')
        {
            break;
        }
        ++pos; // skip '{'

        Snippet snippet;
        snippet.name = name;

        // Parse snippet properties
        while (pos < len && json_string[pos] != '}')
        {
            skip_ws();
            if (pos >= len || json_string[pos] == '}')
            {
                break;
            }
            if (json_string[pos] == ',')
            {
                ++pos;
                continue;
            }

            auto key = parse_string();
            skip_ws();
            if (pos < len && json_string[pos] == ':')
            {
                ++pos;
            }
            skip_ws();

            if (key == "prefix")
            {
                snippet.prefix = parse_string();
            }
            else if (key == "body")
            {
                snippet.body = parse_body();
            }
            else if (key == "description")
            {
                snippet.description = parse_string();
            }
            else if (key == "scope")
            {
                snippet.scope = parse_string();
            }
            else
            {
                // Skip unknown value
                if (pos < len && json_string[pos] == '"')
                {
                    parse_string();
                }
            }
        }

        if (pos < len)
        {
            ++pos; // skip closing '}'
        }

        if (!snippet.prefix.empty())
        {
            register_snippet(std::move(snippet));
            ++count;
        }
    }

    return count;
}

void SnippetEngine::register_from_contributions(
    const std::vector<SnippetContribution>& contributions)
{
    for (const auto& contrib : contributions)
    {
        register_snippet({
            .name = contrib.name,
            .prefix = contrib.trigger,
            .body = contrib.body,
        });
    }
}

auto SnippetEngine::expand(const std::string& prefix) const -> std::optional<std::string>
{
    auto iter = prefix_index_.find(prefix);
    if (iter == prefix_index_.end())
    {
        return std::nullopt;
    }

    return expand_body(snippets_[iter->second].body);
}

auto SnippetEngine::get_snippets() const -> const std::vector<Snippet>&
{
    return snippets_;
}

auto SnippetEngine::get_for_prefix(const std::string& prefix_start) const -> std::vector<Snippet>
{
    std::vector<Snippet> matches;
    for (const auto& snippet : snippets_)
    {
        if (snippet.prefix.size() >= prefix_start.size() &&
            snippet.prefix.compare(0, prefix_start.size(), prefix_start) == 0)
        {
            matches.push_back(snippet);
        }
    }
    return matches;
}

auto SnippetEngine::get_by_name(const std::string& name) const -> const Snippet*
{
    for (const auto& snippet : snippets_)
    {
        if (snippet.name == name)
        {
            return &snippet;
        }
    }
    return nullptr;
}

auto SnippetEngine::count() const -> std::size_t
{
    return snippets_.size();
}

void SnippetEngine::clear()
{
    snippets_.clear();
    prefix_index_.clear();
}

auto SnippetEngine::expand_body(const std::string& body) -> std::string
{
    // Expand a snippet body by resolving tab stops:
    //   $0, $1, $2...           → empty string (cursor positions)
    //   ${1:placeholder}        → placeholder text
    //   ${1|choice1,choice2|}   → first choice
    //   \\$                     → literal $

    std::string result;
    const auto len = body.size();

    for (std::size_t idx = 0; idx < len; ++idx)
    {
        if (body[idx] == '\\' && idx + 1 < len && body[idx + 1] == '$')
        {
            result += '$';
            ++idx; // skip the escaped $
            continue;
        }

        if (body[idx] != '$')
        {
            result += body[idx];
            continue;
        }

        // We have a '$'
        ++idx;
        if (idx >= len)
        {
            result += '$';
            break;
        }

        // Simple tab stop: $0, $1, $2, etc.
        if (body[idx] >= '0' && body[idx] <= '9')
        {
            // Skip the digit(s) — they resolve to empty string
            while (idx < len && body[idx] >= '0' && body[idx] <= '9')
            {
                ++idx;
            }
            --idx; // outer loop will increment
            continue;
        }

        // Complex tab stop: ${...}
        if (body[idx] == '{')
        {
            ++idx; // skip '{'
            // Skip the index digits
            while (idx < len && body[idx] >= '0' && body[idx] <= '9')
            {
                ++idx;
            }

            if (idx < len && body[idx] == ':')
            {
                // Placeholder: ${1:text}
                ++idx; // skip ':'
                std::string placeholder;
                int brace_depth = 1;
                while (idx < len && brace_depth > 0)
                {
                    if (body[idx] == '{')
                    {
                        ++brace_depth;
                    }
                    else if (body[idx] == '}')
                    {
                        --brace_depth;
                        if (brace_depth == 0)
                        {
                            break;
                        }
                    }
                    placeholder += body[idx];
                    ++idx;
                }
                result += placeholder;
                // idx is on closing '}', outer loop will increment past it
            }
            else if (idx < len && body[idx] == '|')
            {
                // Choice: ${1|a,b,c|}
                ++idx; // skip '|'
                std::string first_choice;
                while (idx < len && body[idx] != ',' && body[idx] != '|')
                {
                    first_choice += body[idx];
                    ++idx;
                }
                result += first_choice;
                // Skip remaining choices and closing |}
                while (idx < len && body[idx] != '}')
                {
                    ++idx;
                }
                // idx is on closing '}', outer loop will increment past it
            }
            else
            {
                // Simple ${1} — skip to closing brace
                while (idx < len && body[idx] != '}')
                {
                    ++idx;
                }
                // idx is on closing '}', outer loop will increment past it
            }
            continue;
        }

        // Unrecognized $ usage — keep the $
        result += '$';
        --idx; // re-process the char after $
    }

    return result;
}

} // namespace markamp::core
