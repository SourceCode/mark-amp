#pragma once

/// @file InlineLinkHandler.h
/// @brief V9 Phase 45 Task 8 – Link click handling and classification for WYSIWYG mode.
///
/// Header-only, pure logic. Classifies link URLs into action types and
/// resolves wiki-links and heading references.

#include <cstdint>
#include <string>

namespace markamp::core
{

/// Action to take when a link is clicked in WYSIWYG mode.
enum class LinkClickAction : std::uint8_t
{
    kNavigateInternal = 0, ///< Navigate to another document in the workspace
    kOpenExternal = 1,     ///< Open URL in external browser
    kRevealFile = 2,       ///< Reveal file in file explorer
    kScrollToHeading = 3,  ///< Scroll to a heading anchor in the current document
    kScrollToFootnote = 4, ///< Scroll to a footnote definition
    kShowTooltip = 5       ///< Show link info tooltip (no navigation)
};

/// Result of link classification with resolved target.
struct LinkClassification
{
    LinkClickAction action{LinkClickAction::kShowTooltip};
    std::string resolved_target; ///< Resolved path, URL, or anchor
    std::string display_text;    ///< Human-readable display of what clicking does
    bool is_valid{false};        ///< Whether the link target could be resolved
};

/// Classifies and resolves link URLs for WYSIWYG click handling.
class InlineLinkHandler
{
public:
    InlineLinkHandler() = default;

    /// Classify a link URL into an action type.
    [[nodiscard]] static auto classify_link(const std::string& url,
                                            const std::string& document_path = "")
        -> LinkClassification
    {
        LinkClassification result;

        if (url.empty())
        {
            return result;
        }

        // External URLs
        if (url.find("http://") == 0 || url.find("https://") == 0)
        {
            result.action = LinkClickAction::kOpenExternal;
            result.resolved_target = url;
            result.display_text = "Open " + shorten_url(url);
            result.is_valid = true;
            return result;
        }

        // Mailto
        if (url.find("mailto:") == 0)
        {
            result.action = LinkClickAction::kOpenExternal;
            result.resolved_target = url;
            result.display_text = "Email " + url.substr(7);
            result.is_valid = true;
            return result;
        }

        // Heading anchor: #heading-id
        if (!url.empty() && url[0] == '#')
        {
            result.action = LinkClickAction::kScrollToHeading;
            result.resolved_target = url.substr(1);
            result.display_text = "Jump to heading: " + result.resolved_target;
            result.is_valid = true;
            return result;
        }

        // Footnote reference: [^id]
        if (url.size() >= 2 && url[0] == '^')
        {
            result.action = LinkClickAction::kScrollToFootnote;
            result.resolved_target = url.substr(1);
            result.display_text = "Jump to footnote: " + result.resolved_target;
            result.is_valid = true;
            return result;
        }

        // File protocol
        if (url.find("file://") == 0)
        {
            result.action = LinkClickAction::kRevealFile;
            result.resolved_target = url.substr(7);
            result.display_text = "Reveal file: " + result.resolved_target;
            result.is_valid = true;
            return result;
        }

        // Relative path (internal document navigation)
        if (url.find(".md") != std::string::npos || url.find(".markdown") != std::string::npos)
        {
            result.action = LinkClickAction::kNavigateInternal;
            result.resolved_target = resolve_relative_path(url, document_path);
            result.display_text = "Open: " + url;
            result.is_valid = true;
            return result;
        }

        // Default: treat as internal navigation for relative paths
        result.action = LinkClickAction::kNavigateInternal;
        result.resolved_target = resolve_relative_path(url, document_path);
        result.display_text = "Open: " + url;
        result.is_valid = true;
        return result;
    }

    /// Resolve a wiki-link target ([[target]]) to a file path hint.
    [[nodiscard]] static auto resolve_wiki_link(const std::string& target) -> std::string
    {
        if (target.empty())
        {
            return "";
        }

        // Strip any alias: [[target|alias]] → "target"
        auto pipe_pos = target.find('|');
        std::string file_target =
            (pipe_pos != std::string::npos) ? target.substr(0, pipe_pos) : target;

        // Strip any heading anchor: [[target#heading]] → "target"
        auto hash_pos = file_target.find('#');
        if (hash_pos != std::string::npos)
        {
            file_target = file_target.substr(0, hash_pos);
        }

        // Add .md extension if not present
        if (!file_target.empty() && file_target.find('.') == std::string::npos)
        {
            file_target += ".md";
        }

        return file_target;
    }

    /// Resolve a heading fragment link (#heading-text) to a heading anchor ID.
    [[nodiscard]] static auto resolve_heading_link(const std::string& fragment) -> std::string
    {
        if (fragment.empty())
        {
            return "";
        }

        // Slugify: lowercase, replace spaces with hyphens, remove special chars
        std::string slug;
        for (char chr : fragment)
        {
            if (chr == ' ' || chr == '-')
            {
                if (!slug.empty() && slug.back() != '-')
                {
                    slug += '-';
                }
            }
            else if (std::isalnum(static_cast<unsigned char>(chr)) != 0)
            {
                slug += static_cast<char>(std::tolower(static_cast<unsigned char>(chr)));
            }
        }
        // Remove trailing hyphen
        while (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }

        return slug;
    }

    /// Extract the display text and URL from a markdown link [text](url).
    struct LinkParts
    {
        std::string text;
        std::string url;
        std::string title;
        bool is_valid{false};
    };

    [[nodiscard]] static auto parse_link(const std::string& markdown, std::size_t start_pos = 0)
        -> LinkParts
    {
        LinkParts parts;

        if (start_pos >= markdown.size() || markdown[start_pos] != '[')
        {
            return parts;
        }

        auto text_end = markdown.find("](", start_pos);
        if (text_end == std::string::npos)
        {
            return parts;
        }

        parts.text = markdown.substr(start_pos + 1, text_end - start_pos - 1);

        auto url_start = text_end + 2;
        auto paren_close = markdown.find(')', url_start);
        if (paren_close == std::string::npos)
        {
            return parts;
        }

        auto url_content = markdown.substr(url_start, paren_close - url_start);

        // Check for title
        auto quote_pos = url_content.find('"');
        if (quote_pos != std::string::npos)
        {
            parts.url = url_content.substr(0, quote_pos);
            while (!parts.url.empty() && parts.url.back() == ' ')
            {
                parts.url.pop_back();
            }
            auto title_end = url_content.find('"', quote_pos + 1);
            if (title_end != std::string::npos)
            {
                parts.title = url_content.substr(quote_pos + 1, title_end - quote_pos - 1);
            }
        }
        else
        {
            parts.url = url_content;
        }

        parts.is_valid = !parts.url.empty();
        return parts;
    }

private:
    /// Shorten a URL for display.
    [[nodiscard]] static auto shorten_url(const std::string& url) -> std::string
    {
        if (url.size() <= 50)
        {
            return url;
        }

        // Extract domain
        auto protocol_end = url.find("://");
        if (protocol_end == std::string::npos)
        {
            return url.substr(0, 47) + "...";
        }

        auto domain_start = protocol_end + 3;
        auto domain_end = url.find('/', domain_start);
        if (domain_end == std::string::npos)
        {
            return url;
        }

        auto domain = url.substr(domain_start, domain_end - domain_start);
        return domain + "/...";
    }

    /// Resolve a relative path against a document path.
    [[nodiscard]] static auto resolve_relative_path(const std::string& relative,
                                                    const std::string& document_path) -> std::string
    {
        if (document_path.empty() || relative.empty())
        {
            return relative;
        }

        // If relative path starts with / or is a URL, return as-is
        if (relative[0] == '/' || relative.find("://") != std::string::npos)
        {
            return relative;
        }

        // Find the directory of the document
        auto last_slash = document_path.find_last_of('/');
        if (last_slash == std::string::npos)
        {
            return relative;
        }

        return document_path.substr(0, last_slash + 1) + relative;
    }
};

} // namespace markamp::core
