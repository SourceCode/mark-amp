#include "core/WikiLink.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// WikiLink
// ============================================================================

auto WikiLink::canonical_target() const -> std::string
{
    std::string result = target;
    if (!heading.empty())
    {
        result += '#';
        result += heading;
    }
    if (!block_ref.empty())
    {
        result += '^';
        result += block_ref;
    }
    return result;
}

auto WikiLink::has_heading() const -> bool
{
    return !heading.empty();
}

auto WikiLink::has_block_ref() const -> bool
{
    return !block_ref.empty();
}

// ============================================================================
// ParsedTag
// ============================================================================

auto ParsedTag::root_tag() const -> std::string
{
    if (parts.empty())
    {
        return full_tag;
    }
    return parts.front();
}

auto ParsedTag::depth() const -> int
{
    return static_cast<int>(parts.size());
}

// ============================================================================
// Frontmatter
// ============================================================================

auto Frontmatter::has_field(const std::string& key) const -> bool
{
    // Check known fields
    if (key == "title")
    {
        return !title.empty();
    }
    if (key == "date")
    {
        return !date.empty();
    }
    if (key == "cssclass")
    {
        return !css_class.empty();
    }
    if (key == "publish")
    {
        return true; // always has a value (defaults to false)
    }
    if (key == "tags")
    {
        return !tags.empty();
    }
    if (key == "aliases")
    {
        return !aliases.empty();
    }
    return string_fields.contains(key);
}

auto Frontmatter::get_field(const std::string& key) const -> std::optional<std::string>
{
    if (key == "title")
    {
        return title.empty() ? std::nullopt : std::optional<std::string>(title);
    }
    if (key == "date")
    {
        return date.empty() ? std::nullopt : std::optional<std::string>(date);
    }
    if (key == "cssclass")
    {
        return css_class.empty() ? std::nullopt : std::optional<std::string>(css_class);
    }
    if (key == "publish")
    {
        return publish ? std::optional<std::string>("true") : std::optional<std::string>("false");
    }
    auto field_it = string_fields.find(key);
    if (field_it != string_fields.end())
    {
        return field_it->second;
    }
    return std::nullopt;
}

auto Frontmatter::set_field(const std::string& key, const std::string& value) -> void
{
    if (key == "title")
    {
        title = value;
    }
    else if (key == "date")
    {
        date = value;
    }
    else if (key == "cssclass")
    {
        css_class = value;
    }
    else if (key == "publish")
    {
        publish = (value == "true" || value == "1" || value == "yes");
    }
    else
    {
        string_fields[key] = value;
    }
}

auto Frontmatter::to_yaml() const -> std::string
{
    std::ostringstream oss;
    oss << "---\n";

    if (!title.empty())
    {
        oss << "title: " << title << "\n";
    }
    if (!date.empty())
    {
        oss << "date: " << date << "\n";
    }
    if (!css_class.empty())
    {
        oss << "cssclass: " << css_class << "\n";
    }
    if (publish)
    {
        oss << "publish: true\n";
    }

    if (!tags.empty())
    {
        oss << "tags:\n";
        for (const auto& tag : tags)
        {
            oss << "  - " << tag << "\n";
        }
    }

    if (!aliases.empty())
    {
        oss << "aliases:\n";
        for (const auto& alias : aliases)
        {
            oss << "  - " << alias << "\n";
        }
    }

    // Sort custom fields for deterministic output
    std::vector<std::string> keys;
    keys.reserve(string_fields.size());
    for (const auto& [key, _] : string_fields)
    {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    for (const auto& key : keys)
    {
        oss << key << ": " << string_fields.at(key) << "\n";
    }

    oss << "---\n";
    return oss.str();
}

} // namespace markamp::core
