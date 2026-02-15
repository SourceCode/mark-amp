#include "core/IALParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// Built-in attribute key names per SiYuan spec.
const std::unordered_set<std::string> IALParser::BUILTIN_KEYS =
    { // NOLINT(readability-identifier-naming)
        "id",
        "updated",
        "name",
        "alias",
        "memo",
        "bookmark",
        "style",
        "fold",
        "heading-fold",
        "type",
        "subtype",
        "parent-id"};

auto IALDiff::is_empty() const -> bool
{
    return added.empty() && removed.empty() && changed.empty();
}

auto IALParser::parse(const std::string& ial_string) -> InlineAttributeList
{
    InlineAttributeList result;

    if (ial_string.empty())
    {
        return result;
    }

    // Must start with "{:" and end with "}"
    const auto trimmed = ial_string;
    if (trimmed.size() < 4 || trimmed.substr(0, 2) != "{:" || trimmed.back() != '}')
    {
        return result;
    }

    // Strip delimiters: remove "{:" from front and "}" from back
    const auto inner = trimmed.substr(2, trimmed.size() - 3);

    std::size_t pos = 0;
    const auto len = inner.size();

    while (pos < len)
    {
        // Skip whitespace
        while (pos < len && std::isspace(static_cast<unsigned char>(inner[pos])) != 0)
        {
            ++pos;
        }
        if (pos >= len)
        {
            break;
        }

        // Read key: [a-z][a-z0-9_-]*
        std::size_t key_start = pos;
        while (pos < len && inner[pos] != '=')
        {
            ++pos;
        }
        if (pos >= len)
        {
            break;
        }

        auto key = inner.substr(key_start, pos - key_start);
        // Trim trailing spaces from key
        while (!key.empty() && std::isspace(static_cast<unsigned char>(key.back())) != 0)
        {
            key.pop_back();
        }

        ++pos; // skip '='

        // Skip whitespace after =
        while (pos < len && std::isspace(static_cast<unsigned char>(inner[pos])) != 0)
        {
            ++pos;
        }

        if (pos >= len || inner[pos] != '"')
        {
            break;
        }
        ++pos; // skip opening quote

        // Read value with escape handling
        std::string value;
        while (pos < len)
        {
            if (inner[pos] == '\\' && pos + 1 < len)
            {
                if (inner[pos + 1] == '"')
                {
                    value += '"';
                    pos += 2;
                }
                else if (inner[pos + 1] == '\\')
                {
                    value += '\\';
                    pos += 2;
                }
                else
                {
                    value += inner[pos];
                    ++pos;
                }
            }
            else if (inner[pos] == '"')
            {
                ++pos; // skip closing quote
                break;
            }
            else
            {
                value += inner[pos];
                ++pos;
            }
        }

        if (!key.empty())
        {
            result[key] = value;
        }
    }

    return result;
}

auto IALParser::serialize(const InlineAttributeList& attrs) -> std::string
{
    if (attrs.empty())
    {
        return "";
    }

    // Sort keys alphabetically
    std::vector<std::string> keys;
    keys.reserve(attrs.size());
    for (const auto& [key, _] : attrs)
    {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    std::ostringstream oss;
    oss << "{:";
    for (const auto& key : keys)
    {
        oss << " " << key << "=\"" << escape_value(attrs.at(key)) << "\"";
    }
    oss << " }";
    return oss.str();
}

auto IALParser::merge(const InlineAttributeList& base, const InlineAttributeList& overlay)
    -> InlineAttributeList
{
    auto result = base;
    for (const auto& [key, value] : overlay)
    {
        result[key] = value;
    }
    return result;
}

auto IALParser::diff(const InlineAttributeList& old_attrs, const InlineAttributeList& new_attrs)
    -> IALDiff
{
    IALDiff result;

    // Find added and changed
    for (const auto& [key, new_value] : new_attrs)
    {
        auto old_it = old_attrs.find(key);
        if (old_it == old_attrs.end())
        {
            result.added.emplace_back(key, new_value);
        }
        else if (old_it->second != new_value)
        {
            result.changed.emplace_back(key, old_it->second, new_value);
        }
    }

    // Find removed
    for (const auto& [key, _] : old_attrs)
    {
        if (new_attrs.find(key) == new_attrs.end())
        {
            result.removed.push_back(key);
        }
    }

    return result;
}

auto IALParser::is_valid(const std::string& ial_string) -> bool
{
    if (ial_string.empty())
    {
        return true;
    }

    if (ial_string.size() < 4 || ial_string.substr(0, 2) != "{:" || ial_string.back() != '}')
    {
        return false;
    }

    // Check for empty IAL: "{: }"
    const auto inner = ial_string.substr(2, ial_string.size() - 3);
    bool all_whitespace = true;
    for (auto ch : inner)
    {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0)
        {
            all_whitespace = false;
            break;
        }
    }
    if (all_whitespace)
    {
        return true;
    }

    // Try parsing and verify we get valid results
    auto parsed = parse(ial_string);
    if (parsed.empty() && !all_whitespace)
    {
        return false;
    }

    // Validate all keys
    for (const auto& [key, _] : parsed)
    {
        if (!is_valid_key(key))
        {
            return false;
        }
    }

    return true;
}

auto IALParser::extract_value(const std::string& ial_string, const std::string& key)
    -> std::optional<std::string>
{
    if (ial_string.empty() || key.empty())
    {
        return std::nullopt;
    }

    // Search for key="
    const std::string pattern = key + "=\"";
    auto pos = ial_string.find(pattern);
    if (pos == std::string::npos)
    {
        return std::nullopt;
    }

    pos += pattern.size();

    // Extract value up to next unescaped quote
    std::string value;
    while (pos < ial_string.size())
    {
        if (ial_string[pos] == '\\' && pos + 1 < ial_string.size())
        {
            if (ial_string[pos + 1] == '"')
            {
                value += '"';
                pos += 2;
            }
            else if (ial_string[pos + 1] == '\\')
            {
                value += '\\';
                pos += 2;
            }
            else
            {
                value += ial_string[pos];
                ++pos;
            }
        }
        else if (ial_string[pos] == '"')
        {
            return value;
        }
        else
        {
            value += ial_string[pos];
            ++pos;
        }
    }

    return std::nullopt; // Unterminated value
}

auto IALParser::is_builtin_key(const std::string& key) -> bool
{
    return BUILTIN_KEYS.contains(key);
}

auto IALParser::is_custom_key(const std::string& key) -> bool
{
    return key.starts_with("custom-");
}

auto IALParser::is_valid_key(const std::string& key) -> bool
{
    if (key.empty())
    {
        return false;
    }

    // Must start with lowercase letter
    if (std::islower(static_cast<unsigned char>(key[0])) == 0)
    {
        return false;
    }

    // Remaining chars: lowercase, digits, hyphens, underscores
    for (std::size_t idx = 1; idx < key.size(); ++idx)
    {
        const auto ch = static_cast<unsigned char>(key[idx]);
        if (std::islower(ch) == 0 && std::isdigit(ch) == 0 && key[idx] != '-' && key[idx] != '_')
        {
            return false;
        }
    }

    return true;
}

auto IALParser::escape_value(const std::string& value) -> std::string
{
    std::string result;
    result.reserve(value.size());
    for (char ch : value)
    {
        if (ch == '\\')
        {
            result += "\\\\";
        }
        else if (ch == '"')
        {
            result += "\\\"";
        }
        else
        {
            result += ch;
        }
    }
    return result;
}

auto IALParser::unescape_value(const std::string& value) -> std::string
{
    std::string result;
    result.reserve(value.size());
    for (std::size_t idx = 0; idx < value.size(); ++idx)
    {
        if (value[idx] == '\\' && idx + 1 < value.size())
        {
            if (value[idx + 1] == '"')
            {
                result += '"';
                ++idx;
            }
            else if (value[idx + 1] == '\\')
            {
                result += '\\';
                ++idx;
            }
            else
            {
                result += value[idx];
            }
        }
        else
        {
            result += value[idx];
        }
    }
    return result;
}

} // namespace markamp::core
