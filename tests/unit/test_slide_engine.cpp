// test_slide_engine.cpp — 10 tests for SlideEngine
#include "core/SlideEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SlideEngine starts empty", "[presentation][slide]")
{
    SlideEngine engine;
    CHECK(engine.slide_count() == 0);
}

TEST_CASE("SlideEngine add_slide returns id", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    slide.title = "Introduction";
    auto slide_id = engine.add_slide(slide);
    CHECK_FALSE(slide_id.empty());
    CHECK(engine.slide_count() == 1);
}

TEST_CASE("SlideEngine remove_slide", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    slide.title = "To Remove";
    auto slide_id = engine.add_slide(slide);
    CHECK(engine.remove_slide(slide_id));
    CHECK(engine.slide_count() == 0);
}

TEST_CASE("SlideEngine find_slide", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    slide.title = "Find Me";
    auto slide_id = engine.add_slide(slide);
    auto found = engine.find_slide(slide_id);
    REQUIRE(found != nullptr);
    CHECK(found->title == "Find Me");
}

TEST_CASE("SlideEngine find_slide missing returns null", "[presentation][slide]")
{
    SlideEngine engine;
    CHECK(engine.find_slide("ghost") == nullptr);
}

TEST_CASE("SlideEngine add_block to slide", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    auto slide_id = engine.add_slide(slide);
    SlideBlock block;
    block.block_type = SlideBlockType::kText;
    block.content = "Some text";
    CHECK(engine.add_block(slide_id, block));
    CHECK(engine.block_count(slide_id) == 1);
}

TEST_CASE("SlideEngine set_transition", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    auto slide_id = engine.add_slide(slide);
    CHECK(engine.set_transition(slide_id, SlideTransition::kFade));
    auto found = engine.find_slide(slide_id);
    REQUIRE(found != nullptr);
    CHECK(found->transition == SlideTransition::kFade);
}

TEST_CASE("SlideEngine hide and show slide", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    auto slide_id = engine.add_slide(slide);
    CHECK(engine.hide_slide(slide_id));
    auto found = engine.find_slide(slide_id);
    REQUIRE(found != nullptr);
    CHECK(found->hidden);
    CHECK(engine.show_slide(slide_id));
}

TEST_CASE("SlideEngine set_speaker_notes", "[presentation][slide]")
{
    SlideEngine engine;
    Slide slide;
    auto slide_id = engine.add_slide(slide);
    CHECK(engine.set_speaker_notes(slide_id, "Remember to mention X"));
    auto found = engine.find_slide(slide_id);
    REQUIRE(found != nullptr);
    CHECK(found->speaker_notes == "Remember to mention X");
}

TEST_CASE("SlideEngine clear_all", "[presentation][slide]")
{
    SlideEngine engine;
    Slide s1;
    Slide s2;
    engine.add_slide(s1);
    engine.add_slide(s2);
    engine.clear_all();
    CHECK(engine.slide_count() == 0);
}
