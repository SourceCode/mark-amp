// test_slide_block_types.cpp — 10 tests for SlideBlock and Slide types from SlideEngine
#include "core/SlideEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SlideBlock defaults", "[presentation][slideblock]")
{
    SlideBlock block;
    CHECK(block.block_id.empty());
    CHECK(block.block_type == SlideBlockType::kText);
    CHECK(block.content.empty());
    CHECK(block.sort_order == 0);
    CHECK(block.style_class.empty());
}

TEST_CASE("SlideBlock with title content", "[presentation][slideblock]")
{
    SlideBlock block;
    block.block_type = SlideBlockType::kTitle;
    block.content = "Presentation Title";
    CHECK(block.block_type == SlideBlockType::kTitle);
}

TEST_CASE("SlideBlock code block", "[presentation][slideblock]")
{
    SlideBlock block;
    block.block_type = SlideBlockType::kCodeBlock;
    block.content = "int main() { return 0; }";
    block.style_class = "language-cpp";
    CHECK(block.block_type == SlideBlockType::kCodeBlock);
    CHECK_FALSE(block.style_class.empty());
}

TEST_CASE("SlideBlockType enum covers all types", "[presentation][slideblock]")
{
    CHECK(SlideBlockType::kTitle != SlideBlockType::kSubtitle);
    CHECK(SlideBlockType::kText != SlideBlockType::kBulletList);
    CHECK(SlideBlockType::kCodeBlock != SlideBlockType::kImage);
    CHECK(SlideBlockType::kTable != SlideBlockType::kQuote);
    CHECK(SlideBlockType::kDivider != SlideBlockType::kTitle);
}

TEST_CASE("Slide defaults", "[presentation][slideblock]")
{
    Slide slide;
    CHECK(slide.slide_id.empty());
    CHECK(slide.title.empty());
    CHECK(slide.slide_number == 0);
    CHECK(slide.blocks.empty());
    CHECK(slide.transition == SlideTransition::kNone);
    CHECK_FALSE(slide.hidden);
}

TEST_CASE("Slide with content blocks", "[presentation][slideblock]")
{
    Slide slide;
    slide.title = "Introduction";
    SlideBlock title;
    title.block_type = SlideBlockType::kTitle;
    title.content = "Welcome";
    SlideBlock body;
    body.block_type = SlideBlockType::kBulletList;
    body.content = "- Point 1\n- Point 2";
    slide.blocks.push_back(title);
    slide.blocks.push_back(body);
    CHECK(slide.blocks.size() == 2);
}

TEST_CASE("Slide with background", "[presentation][slideblock]")
{
    Slide slide;
    slide.background_color = "#1a1a2e";
    slide.background_image = "bg-pattern.png";
    CHECK(slide.background_color == "#1a1a2e");
    CHECK_FALSE(slide.background_image.empty());
}

TEST_CASE("SlideTransition enum values", "[presentation][slideblock]")
{
    CHECK(SlideTransition::kNone != SlideTransition::kFade);
    CHECK(SlideTransition::kSlideLeft != SlideTransition::kSlideRight);
    CHECK(SlideTransition::kZoomIn != SlideTransition::kDissolve);
}

TEST_CASE("Slide speaker notes", "[presentation][slideblock]")
{
    Slide slide;
    slide.speaker_notes = "Emphasize the key metrics here.";
    CHECK(slide.speaker_notes.find("metrics") != std::string::npos);
}

TEST_CASE("Slide layout options", "[presentation][slideblock]")
{
    Slide slide;
    slide.layout = "two-column";
    CHECK(slide.layout == "two-column");
    slide.layout = "title-only";
    CHECK(slide.layout == "title-only");
}
