#include "canvas/MetadataPanel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("MetadataPanel: set_object populates system + predefined + custom fields",
          "[metadata_panel]")
{
    MetadataPanel panel;
    std::unordered_map<std::string, std::string> meta = {{"status", "done"},
                                                         {"custom_key", "custom_value"}};
    panel.set_object(42, CanvasObjectType::StickyNote, 5, meta, {"tag1", "tag2"});

    // System: id, type, z_index, tags = 4
    // Predefined: status, priority, author, created, modified = 5
    // Custom: custom_key = 1
    // Total = 10
    REQUIRE(panel.field_count() == 10);
    REQUIRE(panel.current_object_id() == 42);
}

TEST_CASE("MetadataPanel: system fields are read-only", "[metadata_panel]")
{
    MetadataPanel panel;
    panel.set_object(1, CanvasObjectType::TextBox, 0, {}, {});

    // id, type, z_index are system fields
    REQUIRE(panel.field_at(0).key == "id");
    REQUIRE(panel.field_at(0).is_system);
    REQUIRE(panel.field_at(1).key == "type");
    REQUIRE(panel.field_at(1).is_system);
    REQUIRE(panel.field_at(1).value == "TextBox");
    REQUIRE(panel.field_at(2).key == "z_index");
    REQUIRE(panel.field_at(2).is_system);

    // Cannot modify system fields.
    REQUIRE_FALSE(panel.set_field_value("id", "999"));
    REQUIRE_FALSE(panel.remove_field("type"));
}

TEST_CASE("MetadataPanel: add and remove custom field", "[metadata_panel]")
{
    MetadataPanel panel;
    panel.set_object(10, CanvasObjectType::Shape, 3, {}, {});

    const size_t initial_count = panel.field_count();
    panel.add_custom_field("color", "blue");
    REQUIRE(panel.field_count() == initial_count + 1);

    REQUIRE(panel.remove_field("color"));
    REQUIRE(panel.field_count() == initial_count);
}

TEST_CASE("MetadataPanel: set_field_value fires callback", "[metadata_panel]")
{
    MetadataPanel panel;
    panel.set_object(7, CanvasObjectType::Frame, 1, {{"status", "todo"}}, {});

    ObjectId changed_id = kInvalidObjectId;
    std::string changed_key;
    std::string changed_value;
    panel.set_on_field_changed(
        [&](ObjectId obj_id, const std::string& key, const std::string& value)
        {
            changed_id = obj_id;
            changed_key = key;
            changed_value = value;
        });

    REQUIRE(panel.set_field_value("status", "done"));
    REQUIRE(changed_id == 7);
    REQUIRE(changed_key == "status");
    REQUIRE(changed_value == "done");
}

TEST_CASE("MetadataPanel: set_field_type", "[metadata_panel]")
{
    MetadataPanel panel;
    panel.set_object(5, CanvasObjectType::Table, 2, {}, {});
    panel.add_custom_field("due_date", "2026-01-01");

    REQUIRE(panel.set_field_type("due_date", MetadataFieldType::kDate));

    // Find the field and verify type.
    bool found = false;
    for (size_t idx = 0; idx < panel.field_count(); ++idx)
    {
        if (panel.field_at(idx).key == "due_date")
        {
            REQUIRE(panel.field_at(idx).type == MetadataFieldType::kDate);
            found = true;
        }
    }
    REQUIRE(found);
}

TEST_CASE("MetadataPanel: clear resets panel", "[metadata_panel]")
{
    MetadataPanel panel;
    panel.set_object(3, CanvasObjectType::Image, 0, {{"key", "val"}}, {});
    REQUIRE(panel.field_count() > 0);

    panel.clear();
    REQUIRE(panel.field_count() == 0);
    REQUIRE(panel.current_object_id() == kInvalidObjectId);
}
