#pragma once

#include <functional>
#include <string>

namespace markamp::canvas
{

struct BookmarkMetadata;

/// Scrapes Open Graph and standard HTML meta tags from a URL to populate
/// BookmarkMetadata (title, description, image, favicon, site_name).
class MetadataScraper
{
public:
    using OnCompleteCallback = std::function<void(const BookmarkMetadata& metadata)>;

    /// Fetch Open Graph / meta tag metadata from a URL.  Runs asynchronously.
    auto scrape(const std::string& page_url, OnCompleteCallback on_complete) -> void;

    /// Synchronous parse of raw HTML — useful for testing without HTTP.
    [[nodiscard]] auto parse_og_tags(const std::string& html) const -> BookmarkMetadata;

private:
    /// Extract the value of a content attribute from an OG/meta tag pattern.
    [[nodiscard]] static auto extract_meta_content(const std::string& html,
                                                   const std::string& property) -> std::string;

    /// Extract the href from a <link rel="icon"> tag.
    [[nodiscard]] static auto extract_favicon(const std::string& html) -> std::string;

    /// Extract the text between <title> and </title>.
    [[nodiscard]] static auto extract_title_tag(const std::string& html) -> std::string;
};

} // namespace markamp::canvas
