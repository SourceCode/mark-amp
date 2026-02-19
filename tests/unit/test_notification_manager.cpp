// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/NotificationModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_notification(const std::string& nid,
                              const std::string& title,
                              NotificationSeverity severity,
                              const std::string& source) -> NotificationItem
{
    NotificationItem item;
    item.notification_id = nid;
    item.title = title;
    item.severity = severity;
    item.source = source;
    return item;
}

TEST_CASE("Push and query active notifications", "[notification][push]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Build succeeded", NotificationSeverity::kInfo, "System"));
    model.push(make_notification("n2", "Lint warning", NotificationSeverity::kWarning, "Linter"));

    REQUIRE(model.active().size() == 2);
    REQUIRE(model.unread_count() == 2);
}

TEST_CASE("Dismiss removes from active", "[notification][dismiss]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Build succeeded", NotificationSeverity::kInfo, "System"));
    model.dismiss("n1");

    REQUIRE(model.active().empty());
    REQUIRE(model.all().size() == 1); // still in history
}

TEST_CASE("Mark read reduces unread count", "[notification][read]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Info", NotificationSeverity::kInfo, "System"));
    model.mark_read("n1");

    REQUIRE(model.unread_count() == 0);
}

TEST_CASE("Clear history removes read/dismissed", "[notification][history]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Info", NotificationSeverity::kInfo, "System"));
    model.push(make_notification("n2", "Error", NotificationSeverity::kError, "System"));
    model.mark_read("n1");
    model.clear_history();

    REQUIRE(model.all().size() == 1); // n2 still active
}

TEST_CASE("Filter by severity", "[notification][filter]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Info", NotificationSeverity::kInfo, "System"));
    model.push(make_notification("n2", "Error", NotificationSeverity::kError, "Build"));

    REQUIRE(model.by_severity(NotificationSeverity::kError).size() == 1);
}

TEST_CASE("Filter by source", "[notification][filter]")
{
    NotificationModel model;
    model.push(make_notification("n1", "Info", NotificationSeverity::kInfo, "System"));
    model.push(make_notification("n2", "Lint", NotificationSeverity::kWarning, "Linter"));

    REQUIRE(model.by_source("Linter").size() == 1);
}

TEST_CASE("Quiet hours suppresses info and warning", "[notification][quiet]")
{
    NotificationModel model;
    model.set_quiet_hours(true);

    REQUIRE(model.should_suppress(NotificationSeverity::kInfo));
    REQUIRE(model.should_suppress(NotificationSeverity::kWarning));
    REQUIRE_FALSE(model.should_suppress(NotificationSeverity::kError));
}

TEST_CASE("Quiet hours disabled shows all", "[notification][quiet]")
{
    NotificationModel model;
    REQUIRE_FALSE(model.should_suppress(NotificationSeverity::kInfo));
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
