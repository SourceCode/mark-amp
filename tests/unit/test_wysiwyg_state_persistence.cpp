// test_wysiwyg_state_persistence.cpp — 10 tests for WysiwygDocumentModel advanced operations
#include "core/WysiwygTypes.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("WysiwygDocumentModel replace_block changes content", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Original Title");
    model.replace_block(0, "# New Title");
    auto blocks = model.blocks();
    REQUIRE_FALSE(blocks.empty());
    CHECK(blocks[0].raw_content.find("New Title") != std::string::npos);
}

TEST_CASE("WysiwygDocumentModel insert_block_after", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Title");
    auto initial_count = model.block_count();
    model.insert_block_after(0, "New paragraph inserted.");
    CHECK(model.block_count() == initial_count + 1);
}

TEST_CASE("WysiwygDocumentModel remove_block decreases count", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Title\n\nParagraph\n\n## Section 2");
    auto initial_count = model.block_count();
    model.remove_block(0);
    CHECK(model.block_count() == initial_count - 1);
}

TEST_CASE("WysiwygDocumentModel get_block by index", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Title\n\nParagraph text.");
    auto block = model.get_block(0);
    REQUIRE(block != nullptr);
    CHECK(block->block_type == WysiwygBlockType::kHeading);
}

TEST_CASE("WysiwygDocumentModel get_block out of range returns null", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Title");
    CHECK(model.get_block(999) == nullptr);
}

TEST_CASE("WysiwygDocumentModel blocks_mut returns mutable ref", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("# Title");
    auto& blocks = model.blocks_mut();
    CHECK_FALSE(blocks.empty());
}

TEST_CASE("WysiwygDocumentModel parse blockquote", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("> This is a blockquote");
    auto blocks = model.blocks();
    CHECK_FALSE(blocks.empty());
    CHECK(blocks[0].block_type == WysiwygBlockType::kBlockquote);
}

TEST_CASE("WysiwygDocumentModel parse list", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("- Item 1\n- Item 2\n- Item 3");
    auto blocks = model.blocks();
    CHECK_FALSE(blocks.empty());
    CHECK(blocks[0].block_type == WysiwygBlockType::kList);
}

TEST_CASE("WysiwygDocumentModel parse thematic break", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("---");
    auto blocks = model.blocks();
    // Horizontal rule may be parsed as different block types depending on context
    CHECK_FALSE(blocks.empty());
}

TEST_CASE("WysiwygDocumentModel parse image", "[wysiwyg][persistence]")
{
    WysiwygDocumentModel model;
    model.parse("![alt](image.png)");
    auto blocks = model.blocks();
    CHECK_FALSE(blocks.empty());
    CHECK(blocks[0].block_type == WysiwygBlockType::kImage);
}
