#pragma once

#include <map>
#include <set>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Whitelist-based HTML sanitizer to prevent XSS attacks.
///
/// Strips all tags not in the allowed set, removes dangerous attributes
/// (on* event handlers, javascript: URIs), and sanitizes style attributes.
/// Defense-in-depth: applied to all rendered HTML before display.
class HtmlSanitizer
{
public:
    HtmlSanitizer();

    /// Sanitize HTML content, removing dangerous elements and attributes.
    /// Safe HTML passes through unchanged.
    [[nodiscard]] auto sanitize(std::string_view html) const -> std::string;

    /// Add a tag to the allowed set.
    void allow_tag(const std::string& tag);

    /// Add an allowed attribute for a specific tag.
    void allow_attribute(const std::string& tag, const std::string& attribute);

    /// Block a tag (always removed, even if previously allowed).
    void block_tag(const std::string& tag);

private:
    /// Initialize the default whitelist of tags and attributes.
    void initialize_defaults();

    /// Check if a tag is allowed.
    [[nodiscard]] auto is_tag_allowed(std::string_view tag) const -> bool;

    /// Check if an attribute is allowed for the given tag.
    [[nodiscard]] auto is_attribute_allowed(std::string_view tag,
                                            std::string_view attr,
                                            std::string_view value) const -> bool;

    /// Check if a URI scheme is safe (not javascript:, vbscript:, etc.).
    [[nodiscard]] static auto is_safe_uri(std::string_view uri) -> bool;

    /// Check if a style value is safe (no expression(), url(), import).
    [[nodiscard]] static auto is_safe_style(std::string_view style) -> bool;

    /// Sanitize a single HTML tag, returning sanitized version or empty.
    [[nodiscard]] auto sanitize_tag(std::string_view tag_content) const -> std::string;

    std::set<std::string, std::less<>> allowed_tags_;
    std::set<std::string, std::less<>> blocked_tags_;
    std::map<std::string, std::set<std::string, std::less<>>, std::less<>> allowed_attributes_;
};

} // namespace markamp::core
