// test_wysiwyg_types.cpp — 10 tests for WysiwygTypes and WysiwygDocumentModel
#include "core/WysiwygTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("WysiwygBlock defaults", "[wysiwyg][types]")
{
    WysiwygBlock block;
    CHECK(block.block_type == WysiwygBlockType::kParagraph);
    CHECK(block.start_line == 0);
    CHECK(block.raw_content.empty());
}

TEST_CASE("WysiwygBlock line_count", "[wysiwyg][types]")
{
    WysiwygBlock block;
    block.start_line = 5;
    block.end_line = 10;
    CHECK(block.line_count() == 6);
}

TEST_CASE("WysiwygRenderConfig defaults", "[wysiwyg][types]")
{
    WysiwygRenderConfig config;
    CHECK_FALSE(config.show_markers);
    CHECK(config.fold_inactive_syntax);
    CHECK(config.render_images_inline);
    CHECK(config.render_math_inline);
}

TEST_CASE("WysiwygDocumentModel starts empty", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    CHECK(model.block_count() == 0);
    CHECK(model.total_lines() == 0);
}

TEST_CASE("WysiwygDocumentModel parse simple markdown", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    model.parse("# Hello World\n\nSome paragraph text.");
    CHECK(model.block_count() > 0);
}

TEST_CASE("WysiwygDocumentModel parse heading", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    model.parse("# Title\n\nParagraph.");
    auto blocks = model.blocks();
    CHECK_FALSE(blocks.empty());
    CHECK(blocks[0].block_type == WysiwygBlockType::kHeading);
}

TEST_CASE("WysiwygDocumentModel parse code block", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    model.parse("```python\nprint('hello')\n```");
    auto blocks = model.blocks();
    CHECK_FALSE(blocks.empty());
    CHECK(blocks[0].block_type == WysiwygBlockType::kCodeBlock);
}

TEST_CASE("WysiwygDocumentModel to_markdown roundtrips", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    std::string input = "# Title\n\nParagraph text.";
    model.parse(input);
    auto output = model.to_markdown();
    CHECK_FALSE(output.empty());
}

TEST_CASE("WysiwygDocumentModel get_block_at_line", "[wysiwyg][model]")
{
    WysiwygDocumentModel model;
    model.parse("# Title\n\nParagraph\n\n## Sub-heading");
    auto block = model.get_block_at_line(0);
    REQUIRE(block != nullptr);
    CHECK(block->block_type == WysiwygBlockType::kHeading);
}

TEST_CASE("WysiwygBlockType enum values", "[wysiwyg][types]")
{
    CHECK(WysiwygBlockType::kParagraph != WysiwygBlockType::kHeading);
    CHECK(WysiwygBlockType::kCodeBlock != WysiwygBlockType::kBlockquote);
    CHECK(WysiwygBlockType::kList != WysiwygBlockType::kTable);
    CHECK(WysiwygBlockType::kImage != WysiwygBlockType::kCallout);
}
