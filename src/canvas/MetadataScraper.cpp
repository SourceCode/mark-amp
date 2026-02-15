#include "MetadataScraper.h"

#include "canvas/BookmarkCardObject.h"

#include <algorithm>
#include <cctype>
#include <regex>

namespace markamp::canvas
{

auto MetadataScraper::scrape(const std::string& /*page_url*/, OnCompleteCallback /*on_complete*/)
    -> void
{
    // Stub — real implementation would use HttpClient to fetch the URL,
    // then call parse_og_tags on the result and invoke the callback.
}

auto MetadataScraper::parse_og_tags(const std::string& html) const -> BookmarkMetadata
{
    BookmarkMetadata meta;

    meta.title = extract_meta_content(html, "og:title");
    meta.description = extract_meta_content(html, "og:description");
    meta.image_url = extract_meta_content(html, "og:image");
    meta.site_name = extract_meta_content(html, "og:site_name");

    // Fallback: if no OG title, try <title> tag.
    if (meta.title.empty())
    {
        meta.title = extract_title_tag(html);
    }

    // Fallback: if no OG description, try <meta name="description">.
    if (meta.description.empty())
    {
        meta.description = extract_meta_content(html, "description");
    }

    meta.favicon_url = extract_favicon(html);

    return meta;
}

auto MetadataScraper::extract_meta_content(const std::string& html, const std::string& property)
    -> std::string
{
    // Match <meta property="og:title" content="..."> or <meta name="description" content="...">
    // Using a simple regex — sufficient for well-formed HTML.
    const std::string pattern = R"(<meta\s+(?:property|name)\s*=\s*["'])" + property +
                                R"(["']\s+content\s*=\s*["']([^"']*)["'])";

    try
    {
        const std::regex meta_regex(pattern, std::regex::icase);
        std::smatch match;
        if (std::regex_search(html, match, meta_regex) && match.size() > 1)
        {
            return match[1].str();
        }
    }
    catch (const std::regex_error&)
    {
        // Malformed regex — should not happen with our fixed patterns.
    }

    return {};
}

auto MetadataScraper::extract_favicon(const std::string& html) -> std::string
{
    // Match <link rel="icon" href="...">
    try
    {
        const std::regex favicon_regex(
            R"(<link\s+[^>]*rel\s*=\s*["'](?:shortcut )?icon["'][^>]*href\s*=\s*["']([^"']*)["'])",
            std::regex::icase);
        std::smatch match;
        if (std::regex_search(html, match, favicon_regex) && match.size() > 1)
        {
            return match[1].str();
        }
    }
    catch (const std::regex_error&)
    {
    }

    return {};
}

auto MetadataScraper::extract_title_tag(const std::string& html) -> std::string
{
    try
    {
        const std::regex title_regex(R"(<title>([^<]*)</title>)", std::regex::icase);
        std::smatch match;
        if (std::regex_search(html, match, title_regex) && match.size() > 1)
        {
            return match[1].str();
        }
    }
    catch (const std::regex_error&)
    {
    }

    return {};
}

} // namespace markamp::canvas
