/// @file test_clipboard_service.cpp
/// @brief V4 Phase 17 – ClipboardService tests.

#include "core/ClipboardService.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/VaultService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

namespace
{
struct TestFixture
{
    EventBus bus;
    Config config;
    VaultService vault{bus, config};
    ClipboardService service{bus, vault};
};
} // namespace

// ============================================================================
// Test 1: Bold HTML
// ============================================================================
TEST_CASE("HTML bold to Markdown", "[clipboard][bold]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<strong>bold</strong>");
    CHECK(result == "**bold**");
}

// ============================================================================
// Test 2: Italic HTML
// ============================================================================
TEST_CASE("HTML italic to Markdown", "[clipboard][italic]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<em>italic</em>");
    CHECK(result == "*italic*");
}

// ============================================================================
// Test 3: Heading HTML
// ============================================================================
TEST_CASE("HTML heading to Markdown", "[clipboard][heading]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<h2>Title</h2>");
    CHECK(result.find("## Title") != std::string::npos);
}

// ============================================================================
// Test 4: Link HTML
// ============================================================================
TEST_CASE("HTML anchor to Markdown link", "[clipboard][link]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown(R"(<a href="https://example.com">click</a>)");
    CHECK(result == "[click](https://example.com)");
}

// ============================================================================
// Test 5: List HTML
// ============================================================================
TEST_CASE("HTML list to Markdown", "[clipboard][list]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<ul><li>item one</li><li>item two</li></ul>");
    CHECK(result.find("- item one") != std::string::npos);
    CHECK(result.find("- item two") != std::string::npos);
}

// ============================================================================
// Test 6: Table (CSV to Markdown)
// ============================================================================
TEST_CASE("CSV to Markdown table", "[clipboard][csv]")
{
    TestFixture f;
    auto result = f.service.csv_to_markdown_table("Name,Age\nAlice,30\nBob,25");
    CHECK(result.find("| Name | Age |") != std::string::npos);
    CHECK(result.find("| --- |") != std::string::npos);
    CHECK(result.find("| Alice | 30 |") != std::string::npos);
}

// ============================================================================
// Test 7: Nested HTML
// ============================================================================
TEST_CASE("Nested HTML converts correctly", "[clipboard][nested]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<strong><em>bold italic</em></strong>");
    // Should contain both markers
    CHECK(result.find("**") != std::string::npos);
    CHECK(result.find("*") != std::string::npos);
}

// ============================================================================
// Test 8: URL to markdown link
// ============================================================================
TEST_CASE("URL to Markdown link", "[clipboard][url]")
{
    TestFixture f;
    auto result = f.service.url_to_markdown_link("https://example.com/page");
    CHECK(result == "[link](https://example.com/page)");
}

// ============================================================================
// Test 9: Image URL to embed
// ============================================================================
TEST_CASE("Image URL to Markdown embed", "[clipboard][imageurl]")
{
    TestFixture f;
    auto result = f.service.url_to_markdown_link("https://example.com/photo.png");
    CHECK(result == "![image](https://example.com/photo.png)");
}

// ============================================================================
// Test 10: Strip HTML tags
// ============================================================================
TEST_CASE("Strip HTML tags", "[clipboard][strip]")
{
    auto result = ClipboardService::strip_html_tags("<div><p>Hello <b>World</b></p></div>");
    CHECK(result == "Hello World");
}

// ============================================================================
// Test 11: Code block
// ============================================================================
TEST_CASE("HTML pre/code to Markdown code block", "[clipboard][code]")
{
    TestFixture f;
    auto result = f.service.html_to_markdown("<pre>console.log('hi');</pre>");
    CHECK(result.find("```") != std::string::npos);
    CHECK(result.find("console.log") != std::string::npos);
}

// ============================================================================
// Test 12: Content type detection
// ============================================================================
TEST_CASE("Detect paste content type", "[clipboard][detect]")
{
    TestFixture f;

    PasteContent html_content;
    html_content.html = "<b>hello</b>";
    CHECK(f.service.detect_content_type(html_content) == PasteContentType::kHtml);

    PasteContent url_content;
    url_content.url = "https://example.com";
    CHECK(f.service.detect_content_type(url_content) == PasteContentType::kUrl);

    PasteContent csv_content;
    csv_content.text = "a,b\n1,2";
    CHECK(f.service.detect_content_type(csv_content) == PasteContentType::kCsvTable);

    PasteContent plain_content;
    plain_content.text = "plain text";
    CHECK(f.service.detect_content_type(plain_content) == PasteContentType::kPlainText);
}
