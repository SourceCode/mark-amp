/// @file test_phase39_notification_activity.cpp
/// @brief Comprehensive tests for Phase 39 — Notification & Activity System.

#include "core/ActivityCommandProvider.h"
#include "core/ActivityFeed.h"
#include "core/ActivityTimeline.h"
#include "core/CommandRegistry.h"
#include "core/Events.h"
#include "core/NotificationCenter.h"
#include "core/NotificationCommandProvider.h"
#include "core/NotificationFilter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// NotificationCenter Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("NotificationCenter: add and retrieve", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif;
    notif.notification_id = "n1";
    notif.title = "Build complete";
    notif.message = "Build finished successfully";
    notif.source = "build";
    notif.group = "builds";

    center.add_notification(notif);
    REQUIRE(center.notification_count() == 1);

    const auto* found = center.find_notification("n1");
    REQUIRE(found != nullptr);
    REQUIRE(found->title == "Build complete");
    REQUIRE(found->source == "build");
}

TEST_CASE("NotificationCenter: remove notification", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif;
    notif.notification_id = "n1";
    notif.title = "Test";
    center.add_notification(notif);

    REQUIRE(center.remove_notification("n1") == true);
    REQUIRE(center.notification_count() == 0);
    REQUIRE(center.remove_notification("nonexistent") == false);
}

TEST_CASE("NotificationCenter: mark read/unread", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif;
    notif.notification_id = "n1";
    center.add_notification(notif);

    REQUIRE(center.mark_read("n1") == true);
    REQUIRE(center.find_notification("n1")->status == NotificationStatus::kRead);

    REQUIRE(center.mark_unread("n1") == true);
    REQUIRE(center.find_notification("n1")->status == NotificationStatus::kUnread);
}

TEST_CASE("NotificationCenter: dismiss and archive", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif;
    notif.notification_id = "n1";
    center.add_notification(notif);

    REQUIRE(center.dismiss("n1") == true);
    REQUIRE(center.find_notification("n1")->status == NotificationStatus::kDismissed);

    REQUIRE(center.archive("n1") == true);
    REQUIRE(center.find_notification("n1")->status == NotificationStatus::kArchived);
}

TEST_CASE("NotificationCenter: mark all read", "[phase39][notification-center]")
{
    NotificationCenter center;
    for (int idx = 0; idx < 3; ++idx)
    {
        CenterNotification notif;
        notif.notification_id = "n" + std::to_string(idx);
        center.add_notification(notif);
    }

    center.mark_all_read();
    for (const auto& entry : center.all_notifications())
    {
        REQUIRE(entry.status == NotificationStatus::kRead);
    }
}

TEST_CASE("NotificationCenter: dismiss all respects archived", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    center.add_notification(notif2);
    center.archive("n2");

    center.dismiss_all();
    REQUIRE(center.find_notification("n1")->status == NotificationStatus::kDismissed);
    REQUIRE(center.find_notification("n2")->status == NotificationStatus::kArchived);
}

TEST_CASE("NotificationCenter: unread notifications filter", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    center.add_notification(notif2);
    center.mark_read("n2");

    auto unread = center.unread_notifications();
    REQUIRE(unread.size() == 1);
    REQUIRE(unread[0]->notification_id == "n1");
}

TEST_CASE("NotificationCenter: filter by group", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    notif1.group = "sync";
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    notif2.group = "build";
    center.add_notification(notif2);

    auto sync_notifs = center.notifications_by_group("sync");
    REQUIRE(sync_notifs.size() == 1);
    REQUIRE(sync_notifs[0]->notification_id == "n1");
}

TEST_CASE("NotificationCenter: filter by priority", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    notif1.priority = NotificationPriority::kHigh;
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    notif2.priority = NotificationPriority::kLow;
    center.add_notification(notif2);

    auto high = center.notifications_by_priority(NotificationPriority::kHigh);
    REQUIRE(high.size() == 1);
}

TEST_CASE("NotificationCenter: summary", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    notif1.group = "sync";
    notif1.priority = NotificationPriority::kHigh;
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    notif2.group = "build";
    center.add_notification(notif2);

    auto summary = center.get_summary();
    REQUIRE(summary.total == 2);
    REQUIRE(summary.unread == 2);
    REQUIRE(summary.high_priority == 1);
    REQUIRE(summary.groups == 2);
}

TEST_CASE("NotificationCenter: groups list", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    notif1.group = "sync";
    center.add_notification(notif1);

    CenterNotification notif2;
    notif2.notification_id = "n2";
    notif2.group = "build";
    center.add_notification(notif2);

    auto group_list = center.groups();
    REQUIRE(group_list.size() == 2);
}

TEST_CASE("NotificationCenter: max history enforcement", "[phase39][notification-center]")
{
    NotificationCenter center;
    center.set_max_history(3);

    for (int idx = 0; idx < 5; ++idx)
    {
        CenterNotification notif;
        notif.notification_id = "n" + std::to_string(idx);
        center.add_notification(notif);
    }

    REQUIRE(center.notification_count() == 3);
}

TEST_CASE("NotificationCenter: clear dismissed", "[phase39][notification-center]")
{
    NotificationCenter center;
    CenterNotification notif1;
    notif1.notification_id = "n1";
    center.add_notification(notif1);
    center.dismiss("n1");

    CenterNotification notif2;
    notif2.notification_id = "n2";
    center.add_notification(notif2);

    center.clear_dismissed();
    REQUIRE(center.notification_count() == 1);
    REQUIRE(center.find_notification("n2") != nullptr);
}

// ═══════════════════════════════════════════════════════════════════
// NotificationFilter Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("NotificationFilter: add and find rule", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    rule.description = "Test rule";
    rule.mode = FilterMatchMode::kInclude;
    filter.add_rule(rule);

    REQUIRE(filter.rule_count() == 1);
    const auto* found = filter.find_rule("r1");
    REQUIRE(found != nullptr);
    REQUIRE(found->description == "Test rule");
}

TEST_CASE("NotificationFilter: remove rule", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    filter.add_rule(rule);

    REQUIRE(filter.remove_rule("r1") == true);
    REQUIRE(filter.rule_count() == 0);
}

TEST_CASE("NotificationFilter: enable/disable rule", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    filter.add_rule(rule);

    REQUIRE(filter.disable_rule("r1") == true);
    REQUIRE(filter.find_rule("r1")->enabled == false);

    REQUIRE(filter.enable_rule("r1") == true);
    REQUIRE(filter.find_rule("r1")->enabled == true);
}

TEST_CASE("NotificationFilter: exclude rule blocks notification", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    rule.mode = FilterMatchMode::kExclude;
    rule.source_pattern = "debug";
    filter.add_rule(rule);

    CenterNotification notif;
    notif.source = "debug_module";
    notif.priority = NotificationPriority::kNormal;
    REQUIRE(filter.should_show(notif) == false);

    CenterNotification notif2;
    notif2.source = "build_system";
    notif2.priority = NotificationPriority::kNormal;
    REQUIRE(filter.should_show(notif2) == true);
}

TEST_CASE("NotificationFilter: include rules require match", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    rule.mode = FilterMatchMode::kInclude;
    rule.source_pattern = "sync";
    rule.min_priority = NotificationPriority::kLow;
    filter.add_rule(rule);

    CenterNotification notif;
    notif.source = "sync_service";
    notif.priority = NotificationPriority::kNormal;
    REQUIRE(filter.should_show(notif) == true);

    CenterNotification notif2;
    notif2.source = "build_system";
    notif2.priority = NotificationPriority::kNormal;
    REQUIRE(filter.should_show(notif2) == false);
}

TEST_CASE("NotificationFilter: apply_filters", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    NotificationFilterRule rule;
    rule.rule_id = "r1";
    rule.mode = FilterMatchMode::kExclude;
    rule.source_pattern = "debug";
    filter.add_rule(rule);

    CenterNotification notif1;
    notif1.source = "debug";
    notif1.priority = NotificationPriority::kNormal;

    CenterNotification notif2;
    notif2.source = "build";
    notif2.priority = NotificationPriority::kNormal;

    std::vector<const CenterNotification*> all = {&notif1, &notif2};
    auto filtered = filter.apply_filters(all);
    REQUIRE(filtered.size() == 1);
    REQUIRE(filtered[0]->source == "build");
}

TEST_CASE("NotificationFilter: load defaults", "[phase39][notification-filter]")
{
    NotificationFilter filter;
    filter.load_defaults();
    REQUIRE(filter.rule_count() == 2);
}

// ═══════════════════════════════════════════════════════════════════
// ActivityFeed Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ActivityFeed: record and retrieve", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a1";
    entry.description = "Edited file.md";
    entry.category = ActivityCategory::kFileEdit;
    entry.source = "editor";
    entry.target = "/notes/file.md";

    feed.record(entry);
    REQUIRE(feed.entry_count() == 1);

    const auto* found = feed.find_entry("a1");
    REQUIRE(found != nullptr);
    REQUIRE(found->description == "Edited file.md");
}

TEST_CASE("ActivityFeed: filter by category", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    ActivityEntry entry1;
    entry1.activity_id = "a1";
    entry1.category = ActivityCategory::kFileEdit;
    feed.record(entry1);

    ActivityEntry entry2;
    entry2.activity_id = "a2";
    entry2.category = ActivityCategory::kSearch;
    feed.record(entry2);

    auto edits = feed.entries_by_category(ActivityCategory::kFileEdit);
    REQUIRE(edits.size() == 1);
    REQUIRE(edits[0]->activity_id == "a1");
}

TEST_CASE("ActivityFeed: recent entries", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    for (int idx = 0; idx < 5; ++idx)
    {
        ActivityEntry entry;
        entry.activity_id = "a" + std::to_string(idx);
        feed.record(entry);
    }

    auto recent = feed.recent_entries(3);
    REQUIRE(recent.size() == 3);
    REQUIRE(recent[0]->activity_id == "a4"); // Most recent first
}

TEST_CASE("ActivityFeed: search by keyword", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    ActivityEntry entry1;
    entry1.activity_id = "a1";
    entry1.description = "Edited todo.md";
    feed.record(entry1);

    ActivityEntry entry2;
    entry2.activity_id = "a2";
    entry2.description = "Searched for patterns";
    feed.record(entry2);

    auto results = feed.search("todo");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->activity_id == "a1");
}

TEST_CASE("ActivityFeed: pin and unpin", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a1";
    feed.record(entry);

    REQUIRE(feed.pin_entry("a1") == true);
    REQUIRE(feed.pinned_entries().size() == 1);

    REQUIRE(feed.unpin_entry("a1") == true);
    REQUIRE(feed.pinned_entries().empty());
}

TEST_CASE("ActivityFeed: active categories", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    ActivityEntry entry1;
    entry1.activity_id = "a1";
    entry1.category = ActivityCategory::kFileEdit;
    feed.record(entry1);

    ActivityEntry entry2;
    entry2.activity_id = "a2";
    entry2.category = ActivityCategory::kSearch;
    feed.record(entry2);

    auto cats = feed.active_categories();
    REQUIRE(cats.size() == 2);
}

TEST_CASE("ActivityFeed: max entries with pinned protection", "[phase39][activity-feed]")
{
    ActivityFeed feed;
    feed.set_max_entries(3);

    ActivityEntry pinned;
    pinned.activity_id = "pinned1";
    pinned.pinned = true;
    feed.record(pinned);

    for (int idx = 0; idx < 4; ++idx)
    {
        ActivityEntry entry;
        entry.activity_id = "a" + std::to_string(idx);
        feed.record(entry);
    }

    // Pinned entry should survive limit enforcement
    REQUIRE(feed.find_entry("pinned1") != nullptr);
    REQUIRE(feed.entry_count() <= 4); // May have pinned + some recent
}

TEST_CASE("ActivityFeed: category name conversion", "[phase39][activity-feed]")
{
    REQUIRE(activity_category_name(ActivityCategory::kFileEdit) == "file_edit");
    REQUIRE(activity_category_name(ActivityCategory::kSearch) == "search");
    REQUIRE(activity_category_name(ActivityCategory::kSync) == "sync");
    REQUIRE(activity_category_name(ActivityCategory::kPlugin) == "plugin");
}

// ═══════════════════════════════════════════════════════════════════
// ActivityTimeline Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ActivityTimeline: build from feed", "[phase39][activity-timeline]")
{
    ActivityFeed feed;

    ActivityEntry entry1;
    entry1.activity_id = "a1";
    entry1.category = ActivityCategory::kFileEdit;
    entry1.timestamp = std::chrono::system_clock::now();
    feed.record(entry1);

    ActivityEntry entry2;
    entry2.activity_id = "a2";
    entry2.category = ActivityCategory::kSearch;
    entry2.timestamp = std::chrono::system_clock::now();
    feed.record(entry2);

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);

    REQUIRE(timeline.active_days() >= 1);
    REQUIRE(timeline.total_events() == 2);
}

TEST_CASE("ActivityTimeline: most active day", "[phase39][activity-timeline]")
{
    ActivityFeed feed;
    for (int idx = 0; idx < 5; ++idx)
    {
        ActivityEntry entry;
        entry.activity_id = "a" + std::to_string(idx);
        entry.category = ActivityCategory::kFileEdit;
        entry.timestamp = std::chrono::system_clock::now();
        feed.record(entry);
    }

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);

    REQUIRE(!timeline.most_active_day().empty());
}

TEST_CASE("ActivityTimeline: most active category", "[phase39][activity-timeline]")
{
    ActivityFeed feed;
    for (int idx = 0; idx < 3; ++idx)
    {
        ActivityEntry entry;
        entry.activity_id = "edit" + std::to_string(idx);
        entry.category = ActivityCategory::kFileEdit;
        entry.timestamp = std::chrono::system_clock::now();
        feed.record(entry);
    }

    ActivityEntry search_entry;
    search_entry.activity_id = "search1";
    search_entry.category = ActivityCategory::kSearch;
    search_entry.timestamp = std::chrono::system_clock::now();
    feed.record(search_entry);

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);

    REQUIRE(timeline.most_active_category() == ActivityCategory::kFileEdit);
}

TEST_CASE("ActivityTimeline: export JSON", "[phase39][activity-timeline]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a1";
    entry.category = ActivityCategory::kFileEdit;
    entry.timestamp = std::chrono::system_clock::now();
    feed.record(entry);

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);

    auto json = timeline.export_json();
    REQUIRE(json.find("timeline") != std::string::npos);
    REQUIRE(json.find("active_days") != std::string::npos);
    REQUIRE(json.find("total_events") != std::string::npos);
}

TEST_CASE("ActivityTimeline: clear", "[phase39][activity-timeline]")
{
    ActivityFeed feed;
    ActivityEntry entry;
    entry.activity_id = "a1";
    entry.timestamp = std::chrono::system_clock::now();
    feed.record(entry);

    ActivityTimeline timeline;
    timeline.build_from_feed(feed);
    REQUIRE(timeline.active_days() >= 1);

    timeline.clear();
    REQUIRE(timeline.active_days() == 0);
}

// ═══════════════════════════════════════════════════════════════════
// NotificationCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("NotificationCommandProvider: command count", "[phase39][notification-commands]")
{
    REQUIRE(NotificationCommandProvider::command_count() == 8);
}

TEST_CASE("NotificationCommandProvider: command IDs", "[phase39][notification-commands]")
{
    auto ids = NotificationCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "notifications.showCenter");
    REQUIRE(ids[7] == "notifications.archiveAll");
}

TEST_CASE("NotificationCommandProvider: register to registry", "[phase39][notification-commands]")
{
    CommandRegistry registry;
    NotificationCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("notifications.showCenter");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Notifications");
}

// ═══════════════════════════════════════════════════════════════════
// ActivityCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ActivityCommandProvider: command count", "[phase39][activity-commands]")
{
    REQUIRE(ActivityCommandProvider::command_count() == 8);
}

TEST_CASE("ActivityCommandProvider: command IDs", "[phase39][activity-commands]")
{
    auto ids = ActivityCommandProvider::command_ids();
    REQUIRE(ids.size() == 8);
    REQUIRE(ids[0] == "activity.showFeed");
    REQUIRE(ids[7] == "activity.showStreaks");
}

TEST_CASE("ActivityCommandProvider: register to registry", "[phase39][activity-commands]")
{
    CommandRegistry registry;
    ActivityCommandProvider provider;
    provider.register_commands(registry);

    const auto* cmd = registry.get_command("activity.showFeed");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->category == "Activity");
}

// ═══════════════════════════════════════════════════════════════════
// Phase 39 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Phase 39 Events: NotificationAddedEvent", "[phase39][events]")
{
    NotificationAddedEvent event;
    event.notification_id = "n1";
    event.title = "Build done";
    event.source = "build";
    REQUIRE(event.notification_id == "n1");
    REQUIRE(event.title == "Build done");
}

TEST_CASE("Phase 39 Events: NotificationStatusChangedEvent", "[phase39][events]")
{
    NotificationStatusChangedEvent event;
    event.notification_id = "n1";
    event.new_status = "read";
    REQUIRE(event.new_status == "read");
}

TEST_CASE("Phase 39 Events: ActivityRecordedEvent", "[phase39][events]")
{
    ActivityRecordedEvent event;
    event.activity_id = "a1";
    event.description = "Edited file";
    event.category = "file_edit";
    REQUIRE(event.category == "file_edit");
}

TEST_CASE("Phase 39 Events: ActivityTimelineBuiltEvent", "[phase39][events]")
{
    ActivityTimelineBuiltEvent event;
    event.active_days = 30;
    event.total_events = 1500;
    REQUIRE(event.active_days == 30);
    REQUIRE(event.total_events == 1500);
}

TEST_CASE("Phase 39 Events: ActivityPinChangedEvent", "[phase39][events]")
{
    ActivityPinChangedEvent event;
    event.activity_id = "a1";
    event.pinned = true;
    REQUIRE(event.pinned == true);
}
