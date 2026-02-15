#include "canvas/BookmarkCardObject.h"
#include "canvas/MetadataScraper.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("BookmarkCardObject: construction defaults", "[bookmark_card]")
{
    BookmarkCardObject card;
    REQUIRE(card.type() == CanvasObjectType::BookmarkCard);
    REQUIRE(card.card_width() == 320.0);
    REQUIRE(card.card_height() == 180.0);
    REQUIRE(card.show_image());
    REQUIRE(card.url().empty());
}

TEST_CASE("BookmarkCardObject: set URL", "[bookmark_card]")
{
    BookmarkCardObject card;
    card.set_url("https://example.com/article");
    REQUIRE(card.url() == "https://example.com/article");
}

TEST_CASE("BookmarkCardObject: set metadata", "[bookmark_card]")
{
    BookmarkCardObject card;
    BookmarkMetadata meta;
    meta.title = "Example Article";
    meta.description = "A great article";
    meta.site_name = "Example";
    card.set_bookmark_metadata(meta);

    REQUIRE(card.bookmark_metadata().title == "Example Article");
    REQUIRE(card.bookmark_metadata().description == "A great article");
    REQUIRE(card.bookmark_metadata().site_name == "Example");
}

TEST_CASE("BookmarkCardObject: JSON round-trip", "[bookmark_card]")
{
    BookmarkCardObject card;
    card.set_url("https://news.example.com");
    BookmarkMetadata meta;
    meta.title = "News";
    meta.description = "Latest updates";
    card.set_bookmark_metadata(meta);

    const auto json = card.to_json();
    REQUIRE(json.find("news.example.com") != std::string::npos);
    REQUIRE(json.find("News") != std::string::npos);
    REQUIRE(json.find("Latest updates") != std::string::npos);
}

TEST_CASE("MetadataScraper: parse OG tags", "[bookmark_card]")
{
    const std::string html = R"(
        <html><head>
        <meta property="og:title" content="My Page Title">
        <meta property="og:description" content="A description of the page">
        <meta property="og:image" content="https://example.com/image.png">
        <meta property="og:site_name" content="MySite">
        </head></html>
    )";

    MetadataScraper scraper;
    const auto meta = scraper.parse_og_tags(html);

    REQUIRE(meta.title == "My Page Title");
    REQUIRE(meta.description == "A description of the page");
    REQUIRE(meta.image_url == "https://example.com/image.png");
    REQUIRE(meta.site_name == "MySite");
}

TEST_CASE("MetadataScraper: parse fallback title", "[bookmark_card]")
{
    const std::string html = R"(
        <html><head>
        <title>Fallback Title</title>
        <meta name="description" content="Fallback description">
        </head></html>
    )";

    MetadataScraper scraper;
    const auto meta = scraper.parse_og_tags(html);

    REQUIRE(meta.title == "Fallback Title");
    REQUIRE(meta.description == "Fallback description");
}

TEST_CASE("BookmarkCardObject: clone preserves metadata", "[bookmark_card]")
{
    BookmarkCardObject card;
    card.set_url("https://example.com");
    BookmarkMetadata meta;
    meta.title = "Clone Test";
    meta.description = "Testing clone";
    card.set_bookmark_metadata(meta);
    card.set_show_image(false);

    auto cloned = card.clone();
    const auto* cloned_card = dynamic_cast<BookmarkCardObject*>(cloned.get());
    REQUIRE(cloned_card != nullptr);
    REQUIRE(cloned_card->url() == "https://example.com");
    REQUIRE(cloned_card->bookmark_metadata().title == "Clone Test");
    REQUIRE_FALSE(cloned_card->show_image());
}
