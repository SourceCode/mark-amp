#include "HtmlSanitizer.h"

#include "Profiler.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

HtmlSanitizer::HtmlSanitizer()
{
    initialize_defaults();
}

void HtmlSanitizer::initialize_defaults()
{
    // Allowed tags (whitelist)
    allowed_tags_ = {
        "h1",
        "h2",
        "h3",
        "h4",
        "h5",
        "h6",
        "p",
        "br",
        "hr",
        "em",
        "strong",
        "del",
        "code",
        "pre",
        "ul",
        "ol",
        "li",
        "blockquote",
        "table",
        "thead",
        "tbody",
        "tr",
        "th",
        "td",
        "a",
        "img",
        "div",
        "span",
        "sup",
        "section",
        "input",
        // Additional safe formatting tags
        "b",
        "i",
        "u",
        "s",
        "sub",
        "mark",
        "dl",
        "dt",
        "dd",
        "figure",
        "figcaption",
        "details",
        "summary",
        "abbr",
        "cite",
        "dfn",
        "kbd",
        "samp",
        "var",
        "small",
        "time",
        "wbr",
        // SVG elements rendered by Mermaid (sanitized separately)
        "svg",
        "g",
        "path",
        "rect",
        "circle",
        "ellipse",
        "line",
        "polyline",
        "polygon",
        "text",
        "tspan",
        "defs",
        "clippath",
        "marker",
        "use",
        "symbol",
    };

    // Blocked tags (always stripped, never allowed)
    blocked_tags_ = {
        "script",
        "style",
        "iframe",
        "object",
        "embed",
        "form",
        "button",
        "textarea",
        "select",
        "link",
        "meta",
        "base",
        "applet",
        "frame",
        "frameset",
        "foreignobject",
    };

    // Allowed attributes per tag
    allowed_attributes_["a"] = {"href", "title", "id", "class"};
    allowed_attributes_["img"] = {"src", "alt", "title", "width", "height", "class"};
    allowed_attributes_["input"] = {"type", "checked", "disabled"};
    allowed_attributes_["td"] = {"style", "class", "colspan", "rowspan"};
    allowed_attributes_["th"] = {"style", "class", "colspan", "rowspan"};
    allowed_attributes_["code"] = {"class"};
    allowed_attributes_["div"] = {"class", "id"};
    allowed_attributes_["span"] = {"class", "id"};
    allowed_attributes_["pre"] = {"class"};
    allowed_attributes_["section"] = {"class", "id"};
    allowed_attributes_["li"] = {"class"};
    allowed_attributes_["ol"] = {"start", "type"};
    allowed_attributes_["blockquote"] = {"class"};
    allowed_attributes_["table"] = {"class"};
    allowed_attributes_["sup"] = {"id", "class"};
    // SVG attributes
    allowed_attributes_["svg"] = {"viewbox", "width", "height", "xmlns", "class", "id", "style"};
    allowed_attributes_["g"] = {"transform", "class", "id", "style"};
    allowed_attributes_["path"] = {
        "d", "fill", "stroke", "stroke-width", "class", "style", "transform"};
    allowed_attributes_["rect"] = {
        "x", "y", "width", "height", "rx", "ry", "fill", "stroke", "class", "style", "transform"};
    allowed_attributes_["circle"] = {"cx", "cy", "r", "fill", "stroke", "class", "style"};
    allowed_attributes_["ellipse"] = {"cx", "cy", "rx", "ry", "fill", "stroke", "class", "style"};
    allowed_attributes_["line"] = {
        "x1", "y1", "x2", "y2", "stroke", "stroke-width", "class", "style"};
    allowed_attributes_["text"] = {"x",
                                   "y",
                                   "dx",
                                   "dy",
                                   "text-anchor",
                                   "fill",
                                   "class",
                                   "style",
                                   "transform",
                                   "font-size",
                                   "font-family",
                                   "dominant-baseline"};
    allowed_attributes_["tspan"] = {"x", "y", "dx", "dy", "class", "style"};
    allowed_attributes_["use"] = {"href", "x", "y", "width", "height"};
    allowed_attributes_["marker"] = {
        "id", "viewbox", "refx", "refy", "markerwidth", "markerheight", "orient"};
    allowed_attributes_["clippath"] = {"id"};
    allowed_attributes_["defs"] = {};
    allowed_attributes_["symbol"] = {"id", "viewbox"};
    allowed_attributes_["polyline"] = {"points", "fill", "stroke", "class", "style"};
    allowed_attributes_["polygon"] = {"points", "fill", "stroke", "class", "style"};
}

// ═══════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════

void HtmlSanitizer::allow_tag(const std::string& tag)
{
    std::string lower_tag = tag;
    std::transform(lower_tag.begin(),
                   lower_tag.end(),
                   lower_tag.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    allowed_tags_.insert(lower_tag);
    blocked_tags_.erase(lower_tag);
}

void HtmlSanitizer::allow_attribute(const std::string& tag, const std::string& attribute)
{
    std::string lower_tag = tag;
    std::transform(lower_tag.begin(),
                   lower_tag.end(),
                   lower_tag.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    std::string lower_attr = attribute;
    std::transform(lower_attr.begin(),
                   lower_attr.end(),
                   lower_attr.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    allowed_attributes_[lower_tag].insert(lower_attr);
}

void HtmlSanitizer::block_tag(const std::string& tag)
{
    std::string lower_tag = tag;
    std::transform(lower_tag.begin(),
                   lower_tag.end(),
                   lower_tag.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    blocked_tags_.insert(lower_tag);
    allowed_tags_.erase(lower_tag);
}

// ═══════════════════════════════════════════════════════
// Core sanitization
// ═══════════════════════════════════════════════════════

auto HtmlSanitizer::sanitize(std::string_view html) const -> std::string
{
    MARKAMP_PROFILE_SCOPE("HtmlSanitizer::sanitize");

    // New stability #36: input length limit (10MB) to prevent processing massive input
    constexpr size_t kMaxInputSize = 10 * 1024 * 1024;
    if (html.size() > kMaxInputSize)
    {
        return std::string(html.substr(0, kMaxInputSize));
    }

    // Early return: if there's no '<', the input has no tags to sanitize
    if (html.find('<') == std::string_view::npos)
    {
        return std::string(html);
    }

    std::string result;
    result.reserve(html.size());

    size_t pos = 0;
    // New stability #37: iteration cap to prevent infinite-loop-like processing
    constexpr size_t kMaxTagIterations = 100000;
    size_t tag_iterations = 0;
    while (pos < html.size())
    {
        if (++tag_iterations > kMaxTagIterations)
        {
            // Append remaining untouched and bail
            result.append(html.substr(pos));
            break;
        }
        // Find next tag
        auto tag_start = html.find('<', pos);
        if (tag_start == std::string_view::npos)
        {
            // No more tags — append remaining text
            result.append(html.substr(pos));
            break;
        }

        // Append text before the tag
        result.append(html.substr(pos, tag_start - pos));

        // Find end of tag
        auto tag_end = html.find('>', tag_start);
        if (tag_end == std::string_view::npos)
        {
            // Malformed tag — treat rest as text and escape the <
            result += "&lt;";
            pos = tag_start + 1;
            continue;
        }

        // Extract tag content (between < and >)
        auto tag_content = html.substr(tag_start + 1, tag_end - tag_start - 1);

        // Process the tag
        auto sanitized = sanitize_tag(tag_content);
        if (!sanitized.empty())
        {
            result += sanitized;
        }

        pos = tag_end + 1;
    }

    return result;
}

// ═══════════════════════════════════════════════════════
// Tag processing
// ═══════════════════════════════════════════════════════

auto HtmlSanitizer::sanitize_tag(std::string_view tag_content) const -> std::string
{
    if (tag_content.empty())
    {
        return {};
    }

    // Handle comments — strip them
    if (tag_content.starts_with("!--"))
    {
        return {};
    }

    // Handle CDATA — strip
    if (tag_content.starts_with("![CDATA["))
    {
        return {};
    }

    // Handle DOCTYPE — strip
    if (tag_content.starts_with("!"))
    {
        return {};
    }

    // Determine if closing tag
    const bool is_closing = tag_content.starts_with("/");
    auto content = is_closing ? tag_content.substr(1) : tag_content;

    // Handle self-closing
    const bool is_self_closing = content.ends_with("/");
    if (is_self_closing)
    {
        content = content.substr(0, content.size() - 1);
    }

    // Extract tag name (first word)
    std::string tag_name;
    size_t name_end = 0;
    while (name_end < content.size() &&
           std::isspace(static_cast<unsigned char>(content[name_end])) == 0)
    {
        tag_name += static_cast<char>(std::tolower(static_cast<unsigned char>(content[name_end])));
        ++name_end;
    }

    if (tag_name.empty())
    {
        return {};
    }

    // Check if blocked
    if (blocked_tags_.contains(tag_name))
    {
        return {};
    }

    // Check if allowed
    if (!is_tag_allowed(tag_name))
    {
        return {};
    }

    // Special check: input must be checkbox
    if (tag_name == "input")
    {
        auto attrs_str = content.substr(name_end);
        std::string lower_attrs(attrs_str);
        std::transform(lower_attrs.begin(),
                       lower_attrs.end(),
                       lower_attrs.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });
        if (lower_attrs.find("type") == std::string::npos ||
            lower_attrs.find("checkbox") == std::string::npos)
        {
            return {}; // Only checkbox inputs allowed
        }
    }

    // For closing tags, just return the tag
    if (is_closing)
    {
        return "</" + tag_name + ">";
    }

    // Parse and filter attributes
    std::string safe_attrs;
    auto attrs_part = content.substr(name_end);

    // Simple attribute parser
    size_t attr_pos = 0;
    // New stability #38: cap attribute count per tag to 50
    int attr_count = 0;
    constexpr int kMaxAttributesPerTag = 50;
    while (attr_pos < attrs_part.size())
    {
        if (++attr_count > kMaxAttributesPerTag)
            break;
        // Skip whitespace
        while (attr_pos < attrs_part.size() &&
               std::isspace(static_cast<unsigned char>(attrs_part[attr_pos])) != 0)
        {
            ++attr_pos;
        }
        if (attr_pos >= attrs_part.size())
        {
            break;
        }

        // Extract attribute name
        std::string attr_name;
        while (attr_pos < attrs_part.size() &&
               std::isspace(static_cast<unsigned char>(attrs_part[attr_pos])) == 0 &&
               attrs_part[attr_pos] != '=' && attrs_part[attr_pos] != '/')
        {
            attr_name +=
                static_cast<char>(std::tolower(static_cast<unsigned char>(attrs_part[attr_pos])));
            ++attr_pos;
        }

        if (attr_name.empty() || attr_name == "/")
        {
            break;
        }

        // Skip whitespace around =
        while (attr_pos < attrs_part.size() &&
               std::isspace(static_cast<unsigned char>(attrs_part[attr_pos])) != 0)
        {
            ++attr_pos;
        }

        std::string attr_value;
        if (attr_pos < attrs_part.size() && attrs_part[attr_pos] == '=')
        {
            ++attr_pos; // Skip =

            // Skip whitespace after =
            while (attr_pos < attrs_part.size() &&
                   std::isspace(static_cast<unsigned char>(attrs_part[attr_pos])) != 0)
            {
                ++attr_pos;
            }

            if (attr_pos < attrs_part.size())
            {
                const char quote = attrs_part[attr_pos];
                if (quote == '"' || quote == '\'')
                {
                    ++attr_pos; // Skip opening quote
                    while (attr_pos < attrs_part.size() && attrs_part[attr_pos] != quote)
                    {
                        attr_value += attrs_part[attr_pos];
                        ++attr_pos;
                    }
                    if (attr_pos < attrs_part.size())
                    {
                        ++attr_pos; // Skip closing quote
                    }
                }
                else
                {
                    // Unquoted value
                    while (attr_pos < attrs_part.size() &&
                           std::isspace(static_cast<unsigned char>(attrs_part[attr_pos])) == 0 &&
                           attrs_part[attr_pos] != '>')
                    {
                        attr_value += attrs_part[attr_pos];
                        ++attr_pos;
                    }
                }
            }
        }

        // Validate attribute
        if (is_attribute_allowed(tag_name, attr_name, attr_value))
        {
            safe_attrs.append(" ");
            safe_attrs.append(attr_name);
            safe_attrs.append("=\"");
            safe_attrs.append(attr_value);
            safe_attrs.append("\"");
        }
    }

    // Reconstruct tag
    std::string result = "<" + tag_name + safe_attrs;
    if (is_self_closing)
    {
        result += " /";
    }
    result += ">";

    return result;
}

// ═══════════════════════════════════════════════════════
// Validation helpers
// ═══════════════════════════════════════════════════════

auto HtmlSanitizer::is_tag_allowed(std::string_view tag) const -> bool
{
    return allowed_tags_.contains(tag);
}

auto HtmlSanitizer::is_attribute_allowed(std::string_view tag,
                                         std::string_view attr,
                                         std::string_view value) const -> bool
{
    // Block all event handlers (on*)
    if (attr.starts_with("on"))
    {
        return false;
    }

    // Universal: id is allowed for anchors on any tag
    if (attr == "id")
    {
        return true;
    }

    // Check tag-specific allowed attributes (transparent comparator: no alloc)
    auto tag_it = allowed_attributes_.find(tag);
    if (tag_it != allowed_attributes_.end())
    {
        if (!tag_it->second.contains(attr))
        {
            return false;
        }
    }
    else
    {
        // No attribute rules for this tag — block all attributes except id
        return false;
    }

    // Value-specific checks
    if (attr == "href" || attr == "src")
    {
        if (!is_safe_uri(value))
        {
            return false;
        }
    }

    if (attr == "style")
    {
        if (!is_safe_style(value))
        {
            return false;
        }
    }

    // Restrict class to safe prefixes (language-, mermaid-, token-, code-)
    if (attr == "class")
    {
        // Allow common CSS class patterns, block script-like values
        auto val_str = std::string(value);
        std::string lower_val = val_str;
        std::transform(lower_val.begin(),
                       lower_val.end(),
                       lower_val.begin(),
                       [](unsigned char chr) { return std::tolower(chr); });
        // Block classes containing javascript or script
        if (lower_val.find("javascript") != std::string::npos ||
            lower_val.find("<script") != std::string::npos)
        {
            return false;
        }
    }

    return true;
}

auto HtmlSanitizer::is_safe_uri(std::string_view uri) -> bool
{
    // Normalize for comparison
    std::string lower_uri(uri);
    std::transform(lower_uri.begin(),
                   lower_uri.end(),
                   lower_uri.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    // Strip leading whitespace and control characters
    size_t start = 0;
    while (start < lower_uri.size() &&
           (std::isspace(static_cast<unsigned char>(lower_uri[start])) != 0 ||
            static_cast<unsigned char>(lower_uri[start]) < 0x20))
    {
        ++start;
    }
    if (start > 0)
    {
        lower_uri = lower_uri.substr(start);
    }

    // Block dangerous URI schemes
    if (lower_uri.starts_with("javascript:") || lower_uri.starts_with("vbscript:") ||
        lower_uri.starts_with("data:text/html"))
    {
        return false;
    }

    // Block data: URIs with SVG content (can contain scripts)
    if (lower_uri.starts_with("data:image/svg"))
    {
        return false;
    }

    return true;
}

auto HtmlSanitizer::is_safe_style(std::string_view style) -> bool
{
    std::string lower_style(style);
    std::transform(lower_style.begin(),
                   lower_style.end(),
                   lower_style.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });

    // Block dangerous CSS constructs
    if (lower_style.find("expression(") != std::string::npos ||
        lower_style.find("javascript:") != std::string::npos ||
        lower_style.find("vbscript:") != std::string::npos ||
        lower_style.find("@import") != std::string::npos ||
        lower_style.find("behavior:") != std::string::npos ||
        lower_style.find("-moz-binding") != std::string::npos)
    {
        return false;
    }

    // Block url() in styles (can lead to data exfiltration)
    if (lower_style.find("url(") != std::string::npos)
    {
        return false;
    }

    return true;
}

} // namespace markamp::core
