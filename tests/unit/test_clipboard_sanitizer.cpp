// test_clipboard_sanitizer.cpp — 10 tests for ClipboardSanitizer
#include "core/ClipboardSanitizer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ClipboardSanitizer sanitize_text returns clean content", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_text("Hello World");
    CHECK(result.content == "Hello World");
    CHECK(result.content_type == ClipboardContentType::kPlainText);
}

TEST_CASE("ClipboardSanitizer sanitize_html strips scripts", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_html("<p>Hello</p><script>alert(1)</script>");
    CHECK(result.content.find("<script>") == std::string::npos);
    CHECK(result.had_dangerous_content);
}

TEST_CASE("ClipboardSanitizer sanitize_markdown processes markdown", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto result = sanitizer.sanitize_markdown("# Title\n\nSome content");
    CHECK_FALSE(result.content.empty());
    CHECK(result.content_type == ClipboardContentType::kMarkdown);
}

TEST_CASE("ClipboardSanitizer sanitize_auto detects type", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    auto html_result = sanitizer.sanitize_auto("<div>HTML content</div>");
    CHECK(html_result.content_type == ClipboardContentType::kHtml);
}

TEST_CASE("ClipboardSanitizer detect_content_type plain text", "[security][clipboard]")
{
    auto content_type = ClipboardSanitizer::detect_content_type("Hello World plain text");
    CHECK(content_type == ClipboardContentType::kPlainText);
}

TEST_CASE("ClipboardSanitizer detect_content_type HTML", "[security][clipboard]")
{
    auto content_type = ClipboardSanitizer::detect_content_type("<div>HTML</div>");
    CHECK(content_type == ClipboardContentType::kHtml);
}

TEST_CASE("ClipboardSanitizer max_paste_size default", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    CHECK(sanitizer.max_paste_size() == 1024ULL * 1024ULL);
}

TEST_CASE("ClipboardSanitizer set_max_paste_size", "[security][clipboard]")
{
    ClipboardSanitizer sanitizer;
    sanitizer.set_max_paste_size(512 * 1024);
    CHECK(sanitizer.max_paste_size() == 512 * 1024);
}

TEST_CASE("ClipboardSanitizer content_type_name returns readable names", "[security][clipboard]")
{
    auto plain = ClipboardSanitizer::content_type_name(ClipboardContentType::kPlainText);
    auto html = ClipboardSanitizer::content_type_name(ClipboardContentType::kHtml);
    auto markdown = ClipboardSanitizer::content_type_name(ClipboardContentType::kMarkdown);
    CHECK_FALSE(plain.empty());
    CHECK_FALSE(html.empty());
    CHECK_FALSE(markdown.empty());
    CHECK(plain != html);
}

TEST_CASE("ClipboardSanitizer SanitizeResult defaults", "[security][clipboard]")
{
    SanitizeResult result;
    CHECK(result.content.empty());
    CHECK(result.content_type == ClipboardContentType::kPlainText);
    CHECK(result.original_length == 0);
    CHECK(result.sanitized_length == 0);
    CHECK(result.modifications == 0);
    CHECK_FALSE(result.was_truncated);
    CHECK_FALSE(result.had_dangerous_content);
}
