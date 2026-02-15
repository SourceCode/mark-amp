#include "canvas/Board.h"
#include "canvas/CanvasSearch.h"
#include "canvas/FrameObject.h"
#include "canvas/SearchBar.h"
#include "canvas/ShapeData.h"
#include "canvas/StickyNote.h"
#include "canvas/TableObject.h"
#include "canvas/TextBox.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("CanvasSearch: search sticky note", "[canvas_search]")
{
    Board board;
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_text("Hello World");
    board.add_object(std::move(sticky));

    auto results = CanvasSearch::search(board, "world");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].matched_text == "World");
}

TEST_CASE("CanvasSearch: case insensitive search", "[canvas_search]")
{
    Board board;
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_text("Hello World");
    board.add_object(std::move(sticky));

    auto results = CanvasSearch::search(board, "hello", false);
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].matched_text == "Hello");
}

TEST_CASE("CanvasSearch: search text box", "[canvas_search]")
{
    Board board;
    auto text_box = std::make_unique<TextBox>();
    text_box->set_text("Formatted document text");
    board.add_object(std::move(text_box));

    auto results = CanvasSearch::search(board, "document");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].match_offset == 10);
}

TEST_CASE("CanvasSearch: search table cells", "[canvas_search]")
{
    Board board;
    auto table = std::make_unique<TableObject>();
    table->set_cell(0, 0, "Revenue");
    table->set_cell(0, 1, "Expenses");
    board.add_object(std::move(table));

    auto results = CanvasSearch::search(board, "revenue");
    REQUIRE(results.size() == 1);
}

TEST_CASE("CanvasSearch: search shape label", "[canvas_search]")
{
    Board board;
    auto shape = std::make_unique<ShapeObject>();
    shape->set_text("Process Step");
    board.add_object(std::move(shape));

    auto results = CanvasSearch::search(board, "process");
    REQUIRE(results.size() == 1);
}

TEST_CASE("CanvasSearch: no results", "[canvas_search]")
{
    Board board;
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_text("Something");
    board.add_object(std::move(sticky));

    auto results = CanvasSearch::search(board, "nonexistent");
    REQUIRE(results.empty());
}

TEST_CASE("CanvasSearch: multiple results across objects", "[canvas_search]")
{
    Board board;
    for (int idx = 0; idx < 3; ++idx)
    {
        auto sticky = std::make_unique<StickyNote>();
        sticky->set_text("test content " + std::to_string(idx));
        board.add_object(std::move(sticky));
    }

    auto results = CanvasSearch::search(board, "test");
    REQUIRE(results.size() == 3);
}

TEST_CASE("CanvasSearch: extract_text per type", "[canvas_search]")
{
    StickyNote sticky;
    sticky.set_text("sticky text");
    REQUIRE(CanvasSearch::extract_text(sticky) == "sticky text");

    TextBox textbox;
    textbox.set_text("textbox text");
    REQUIRE(CanvasSearch::extract_text(textbox) == "textbox text");

    FrameObject frame;
    frame.set_title("my frame");
    REQUIRE(CanvasSearch::extract_text(frame) == "my frame");
}

TEST_CASE("CanvasSearch: context snippet", "[canvas_search]")
{
    Board board;
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_text("The quick brown fox jumps over the lazy dog");
    board.add_object(std::move(sticky));

    auto results = CanvasSearch::search(board, "fox");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].context.find("fox") != std::string::npos);
    REQUIRE(results[0].context.size() <= results[0].matched_text.size() + 60 + 10);
}

TEST_CASE("SearchBar: navigate results", "[canvas_search]")
{
    SearchBar bar;
    std::vector<SearchResult> results = {
        {.object_id = 1, .matched_text = "a", .context = "a", .match_offset = 0, .match_length = 1},
        {.object_id = 2, .matched_text = "b", .context = "b", .match_offset = 0, .match_length = 1},
        {.object_id = 3, .matched_text = "c", .context = "c", .match_offset = 0, .match_length = 1},
    };
    bar.set_results(results);

    REQUIRE(bar.result_count() == 3);
    REQUIRE(bar.current_index() == 0);

    bar.next_result();
    REQUIRE(bar.current_index() == 1);

    bar.next_result();
    REQUIRE(bar.current_index() == 2);

    bar.next_result(); // wraps
    REQUIRE(bar.current_index() == 0);

    bar.previous_result(); // wraps back
    REQUIRE(bar.current_index() == 2);
}
