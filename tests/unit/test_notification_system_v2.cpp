/// @file test_notification_system_v2.cpp
/// @brief Phase 43 — Unit tests for Notification Center model.

#include "ui/NotificationCenterModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("NotificationCenterModel: add and count", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id1 = model.add({.title = "Build complete", .category = NotificationCategory::kBuild});
    auto id2 = model.add({.title = "Git push done", .category = NotificationCategory::kGit});

    REQUIRE(id1 != id2);
    REQUIRE(model.total_count() == 2);
    REQUIRE(model.unread_count() == 2);
}

TEST_CASE("NotificationCenterModel: mark read", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id = model.add({.title = "Test"});
    REQUIRE(model.unread_count() == 1);
    model.mark_read(id);
    REQUIRE(model.unread_count() == 0);
}

TEST_CASE("NotificationCenterModel: mark all read", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "A"});
    model.add({.title = "B"});
    model.add({.title = "C"});
    REQUIRE(model.unread_count() == 3);
    model.mark_all_read();
    REQUIRE(model.unread_count() == 0);
}

TEST_CASE("NotificationCenterModel: dismiss excludes from total", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id = model.add({.title = "Test"});
    model.dismiss(id);
    REQUIRE(model.total_count() == 0);
    REQUIRE(model.all_items().empty());
}

TEST_CASE("NotificationCenterModel: remove permanently", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id = model.add({.title = "Test"});
    model.remove(id);
    REQUIRE(model.find(id) == nullptr);
}

TEST_CASE("NotificationCenterModel: filter by category", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "Build 1", .category = NotificationCategory::kBuild});
    model.add({.title = "Git 1", .category = NotificationCategory::kGit});
    model.add({.title = "Build 2", .category = NotificationCategory::kBuild});

    auto builds = model.items_by_category(NotificationCategory::kBuild);
    REQUIRE(builds.size() == 2);
}

TEST_CASE("NotificationCenterModel: filter by source", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "From Ext", .source = "my-extension"});
    model.add({.title = "System", .source = "system"});

    auto ext = model.items_by_source("my-extension");
    REQUIRE(ext.size() == 1);
    REQUIRE(ext[0].title == "From Ext");
}

TEST_CASE("NotificationCenterModel: grouping", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "Error 1", .group_key = "build-errors"});
    model.add({.title = "Error 2", .group_key = "build-errors"});
    model.add({.title = "Warning", .group_key = "build-warnings"});

    auto groups = model.groups();
    REQUIRE(groups.size() == 2);
}

TEST_CASE("NotificationCenterModel: DND blocks normal", "[notification_v2]")
{
    NotificationCenterModel model;
    model.set_dnd_enabled(true);
    auto id = model.add(
        {.title = "Normal", .priority = NotificationPriority::kNormal, .timestamp_ms = 1000});
    REQUIRE_FALSE(model.should_show(id, 1000));
}

TEST_CASE("NotificationCenterModel: urgent bypasses DND", "[notification_v2]")
{
    NotificationCenterModel model;
    model.set_dnd_enabled(true);
    auto id = model.add(
        {.title = "Urgent!", .priority = NotificationPriority::kUrgent, .timestamp_ms = 1000});
    REQUIRE(model.should_show(id, 1000));
}

TEST_CASE("NotificationCenterModel: snooze hides temporarily", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id = model.add({.title = "Snoozed", .timestamp_ms = 1000});
    model.snooze(id, 5000);                     // snooze for 5 seconds
    REQUIRE_FALSE(model.should_show(id, 2000)); // still snoozed
    REQUIRE(model.should_show(id, 7000));       // snooze expired
}

TEST_CASE("NotificationCenterModel: expiry and cleanup", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "Short-lived", .timestamp_ms = 1000, .expiry_ms = 5000});
    model.add({.title = "Permanent", .timestamp_ms = 1000});

    REQUIRE(model.total_count() == 2);
    model.cleanup_expired(10000);
    REQUIRE(model.total_count() == 1);
}

TEST_CASE("NotificationCenterModel: progress notification", "[notification_v2]")
{
    NotificationCenterModel model;
    auto id = model.add({.title = "Indexing", .progress = 0.5});
    auto* item = model.find(id);
    REQUIRE(item != nullptr);
    REQUIRE(item->progress == 0.5);
}

TEST_CASE("NotificationCenterModel: ordering newest first", "[notification_v2]")
{
    NotificationCenterModel model;
    model.add({.title = "Old", .timestamp_ms = 100});
    model.add({.title = "New", .timestamp_ms = 200});

    auto items = model.all_items();
    REQUIRE(items[0].title == "New");
    REQUIRE(items[1].title == "Old");
}
