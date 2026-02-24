#include "ui/ActivityBarModel.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::ui;

TEST_CASE("ActivityBarModel Dynamic Item Registration", "[ActivityBar][Phase07]")
{
    ActivityBarModel model;

    SECTION("It registers new items dynamically")
    {
        REQUIRE(model.item_count() == 0);
        model.add_item({"explorer", "Explorer", "", "Explorer", "activity-explorer", false});
        model.add_item({"search", "Search", "", "Search", "activity-search", false});

        REQUIRE(model.item_count() == 2);

        auto visible = model.visible_items();
        REQUIRE(visible.size() == 2);
        REQUIRE(visible[0].item_id == "explorer");
        REQUIRE(visible[1].item_id == "search");
    }

    SECTION("It unregisters items")
    {
        model.add_item({"plugin1", "Plugin 1", "", "Plugin 1", "icon-1", false});
        REQUIRE(model.item_count() == 1);

        model.remove_item("plugin1");
        REQUIRE(model.item_count() == 0);
    }
}

TEST_CASE("ActivityBarModel Persistence Standardization", "[ActivityBar][Phase07][Serialization]")
{
    ActivityBarModel model;
    model.add_item({"search", "Search", "", "Search", "activity-search", false});
    model.add_item({"explorer", "Explorer", "", "Explorer", "activity-explorer", false});
    model.add_item({"settings", "Settings", "", "Settings", "activity-settings", true});

    SECTION("It returns current layout for serialization")
    {
        auto layout = model.get_layout();
        REQUIRE(layout.size() == 3);
        REQUIRE(layout[0].first == "search");
        REQUIRE(layout[0].second == true);
        REQUIRE(layout[1].first == "explorer");
        REQUIRE(layout[2].first == "settings");
    }

    SECTION("It applies layout to reorder items and toggle visibility")
    {
        std::vector<std::pair<std::string, bool>> new_layout = {
            {"settings", true}, {"search", false}, {"explorer", true}};

        model.apply_layout(new_layout);

        auto all = model.all_items();
        REQUIRE(all.size() == 3);

        // Check structural order
        REQUIRE(all[0].item_id == "settings");
        REQUIRE(all[1].item_id == "search");
        REQUIRE(all[2].item_id == "explorer");

        // Check visibility applied
        REQUIRE(all[0].visible == true);
        REQUIRE(all[1].visible == false);
        REQUIRE(all[2].visible == true);

        // Check visible_items
        auto vis = model.visible_items();
        REQUIRE(vis.size() == 2);
        REQUIRE(vis[0].item_id == "settings");
        REQUIRE(vis[1].item_id == "explorer");
    }

    SECTION("It appends unknown items missing from layout")
    {
        // Add a late-binding dynamic extension that isn't in layout config
        model.add_item({"new-plugin", "Plugin", "", "P", "icon", false});

        std::vector<std::pair<std::string, bool>> new_layout = {{"explorer", true},
                                                                {"search", true}};

        model.apply_layout(new_layout);

        auto all = model.all_items();
        // new-plugin should be appended
        REQUIRE(all.size() == 4);
        REQUIRE(all[0].item_id == "explorer");
        REQUIRE(all[1].item_id == "search");
        REQUIRE(all[2].item_id == "settings");   // also missing, so appended
        REQUIRE(all[3].item_id == "new-plugin"); // recently added, appended
    }
}

TEST_CASE("ActivityBarModel Badge Source Sync and Math", "[ActivityBar][Phase07]")
{
    ActivityBarModel model;
    model.add_item({"extensions", "Extensions", "", "Extensions", "activity-ext", false});

    SECTION("It handles count-based badges")
    {
        model.set_badge("extensions", BadgeStyle::kCount, 5);
        auto all = model.all_items();
        REQUIRE(all[0].badge_count == 5);
        REQUIRE(all[0].badge_style == BadgeStyle::kCount);

        REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kCount, 5) == "5");
        REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kCount, 150) == "99+");
    }

    SECTION("It handles dot-based badges")
    {
        model.set_badge("extensions", BadgeStyle::kDot, 0);
        auto all = model.all_items();
        REQUIRE(all[0].badge_style == BadgeStyle::kDot);

        REQUIRE(ActivityBarModel::badge_display(BadgeStyle::kDot, 0) == " ");
    }
}
