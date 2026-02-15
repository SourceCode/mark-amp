#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/StickyNote.h"
#include "canvas/TagManager.h"
#include "canvas/TagPanel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// Helper: create a sticky note and add tags.
static auto make_tagged_sticky(const std::string& text, const std::vector<std::string>& tags)
    -> std::unique_ptr<StickyNote>
{
    auto sticky = std::make_unique<StickyNote>();
    sticky->set_text(text);
    for (const auto& tag : tags)
    {
        sticky->add_tag(tag);
    }
    return sticky;
}

TEST_CASE("TagManager: rebuild_index populates tag counts", "[tag_manager]")
{
    Board board;
    board.add_object(make_tagged_sticky("A", {"urgent", "project"}));
    board.add_object(make_tagged_sticky("B", {"urgent"}));
    board.add_object(make_tagged_sticky("C", {"project", "design"}));

    TagManager manager;
    manager.rebuild_index(board);

    REQUIRE(manager.tag_count() == 3);

    auto tags = manager.all_tags();
    // "urgent" and "project" both have count 2, should appear before "design" (count 1).
    REQUIRE(tags[0].usage_count == 2);
    REQUIRE(tags[1].usage_count == 2);
    REQUIRE(tags[2].usage_count == 1);
    REQUIRE(tags[2].name == "design");
}

TEST_CASE("TagManager: objects_with_tag returns correct IDs", "[tag_manager]")
{
    Board board;
    auto id1 = board.add_object(make_tagged_sticky("A", {"alpha"}));
    board.add_object(make_tagged_sticky("B", {"beta"}));
    auto id3 = board.add_object(make_tagged_sticky("C", {"alpha"}));

    TagManager manager;
    manager.rebuild_index(board);

    auto alpha_objs = manager.objects_with_tag("alpha");
    REQUIRE(alpha_objs.size() == 2);

    // Both id1 and id3 should be in the set.
    bool has_id1 = std::find(alpha_objs.begin(), alpha_objs.end(), id1) != alpha_objs.end();
    bool has_id3 = std::find(alpha_objs.begin(), alpha_objs.end(), id3) != alpha_objs.end();
    REQUIRE(has_id1);
    REQUIRE(has_id3);
}

TEST_CASE("TagManager: set and get tag color", "[tag_manager]")
{
    Board board;
    board.add_object(make_tagged_sticky("A", {"red_tag"}));

    TagManager manager;
    manager.rebuild_index(board);

    manager.set_tag_color("red_tag", {255, 0, 0, 255});
    auto color = manager.tag_color("red_tag");
    REQUIRE(color.r == 255);
    REQUIRE(color.g == 0);
    REQUIRE(color.b == 0);
}

TEST_CASE("TagManager: suggest_tags prefix match", "[tag_manager]")
{
    Board board;
    board.add_object(make_tagged_sticky("A", {"project", "prototype", "design"}));

    TagManager manager;
    manager.rebuild_index(board);

    auto suggestions = manager.suggest_tags("pro");
    REQUIRE(suggestions.size() == 2);
    // Should contain "project" and "prototype", sorted alphabetically.
    REQUIRE(suggestions[0] == "project");
    REQUIRE(suggestions[1] == "prototype");
}

TEST_CASE("TagManager: empty tags on object", "[tag_manager]")
{
    Board board;
    board.add_object(std::make_unique<StickyNote>()); // no tags

    TagManager manager;
    manager.rebuild_index(board);

    REQUIRE(manager.tag_count() == 0);
    REQUIRE(manager.all_tags().empty());
}

TEST_CASE("TagManager: objects_with_tag on non-existent tag", "[tag_manager]")
{
    Board board;
    board.add_object(make_tagged_sticky("A", {"real_tag"}));

    TagManager manager;
    manager.rebuild_index(board);

    auto results = manager.objects_with_tag("fake_tag");
    REQUIRE(results.empty());
}

TEST_CASE("TagManager: rebuild preserves custom colors", "[tag_manager]")
{
    Board board;
    board.add_object(make_tagged_sticky("A", {"cool"}));

    TagManager manager;
    manager.rebuild_index(board);
    manager.set_tag_color("cool", {0, 255, 0, 255});

    // Add another object and rebuild.
    board.add_object(make_tagged_sticky("B", {"cool"}));
    manager.rebuild_index(board);

    auto color = manager.tag_color("cool");
    REQUIRE(color.g == 255);
}

TEST_CASE("TagPanel: filter callback fires", "[tag_manager]")
{
    TagPanel panel;
    panel.set_tags({{.name = "important", .color = {255, 0, 0, 255}, .usage_count = 3}});

    std::string filtered_tag;
    panel.set_on_filter_by_tag([&](const std::string& tag) { filtered_tag = tag; });

    panel.set_active_filter("important");
    REQUIRE(filtered_tag == "important");
    REQUIRE(panel.active_filter() == "important");

    panel.clear_filter();
    REQUIRE(panel.active_filter().empty());
}
