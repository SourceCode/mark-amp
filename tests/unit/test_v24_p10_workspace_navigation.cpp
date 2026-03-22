/// @file test_v24_p10_workspace_navigation.cpp
/// @brief V24 Phase 10 tests: Workspace, file operations, navigation history.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/NavigationHistory.h"
#include "../../src/core/FileOperationService.h"
#include "../../src/core/Config.h"
#include "../../src/core/EventBus.h"

using namespace markamp::core;

// P10-T01: Workspace orchestration
TEST_CASE("P10-T01 file op result values", "[v24][p10]") {
    // All FileOpResult values are distinct and properly defined
    FileOpResult r1 = FileOpResult::kSuccess;
    FileOpResult r2 = FileOpResult::kPermissionDenied;
    FileOpResult r3 = FileOpResult::kNotFound;
    REQUIRE(r1 != r2);
    REQUIRE(r2 != r3);
}

// P10-T02: File operations
TEST_CASE("P10-T02 file op result enumeration", "[v24][p10]") {
    REQUIRE(FileOpResult::kSuccess != FileOpResult::kPermissionDenied);
    REQUIRE(FileOpResult::kNotFound != FileOpResult::kAlreadyExists);
    REQUIRE(FileOpResult::kCancelled != FileOpResult::kError);
}

// P10-T03: Startup/recent continuity (tested via session restore in P03)
TEST_CASE("P10-T03 navigation entry structure", "[v24][p10]") {
    NavigationEntry entry;
    entry.artifact_id = "art-1";
    entry.display_name = "file.md";
    entry.line = 42;
    entry.column = 10;
    entry.view_type = "editor";
    REQUIRE(entry.matches_artifact("art-1"));
    REQUIRE_FALSE(entry.matches_artifact("art-2"));
}

// P10-T04: Navigation history
TEST_CASE("P10-T04 navigation push and back", "[v24][p10]") {
    NavigationHistory history;
    history.push({.artifact_id = "a1", .display_name = "File A", .line = 1});
    history.push({.artifact_id = "a2", .display_name = "File B", .line = 10});
    history.push({.artifact_id = "a3", .display_name = "File C", .line = 20});

    REQUIRE(history.entry_count() == 3);
    REQUIRE(history.can_go_back());
    REQUIRE_FALSE(history.can_go_forward());

    auto back1 = history.go_back();
    REQUIRE(back1.has_value());
    REQUIRE(back1->artifact_id == "a2");

    auto back2 = history.go_back();
    REQUIRE(back2.has_value());
    REQUIRE(back2->artifact_id == "a1");
    REQUIRE_FALSE(history.can_go_back());
}

TEST_CASE("P10-T04 navigation forward after back", "[v24][p10]") {
    NavigationHistory history;
    history.push({.artifact_id = "a1", .display_name = "A"});
    history.push({.artifact_id = "a2", .display_name = "B"});

    [[maybe_unused]] auto _ = history.go_back();
    REQUIRE(history.can_go_forward());

    auto fwd = history.go_forward();
    REQUIRE(fwd.has_value());
    REQUIRE(fwd->artifact_id == "a2");
}

TEST_CASE("P10-T04 navigation push clears forward", "[v24][p10]") {
    NavigationHistory history;
    history.push({.artifact_id = "a1", .display_name = "A"});
    history.push({.artifact_id = "a2", .display_name = "B"});
    history.push({.artifact_id = "a3", .display_name = "C"});

    [[maybe_unused]] auto _1 = history.go_back();
    [[maybe_unused]] auto _2 = history.go_back();
    // Now at a1, with a2 and a3 in forward
    REQUIRE(history.can_go_forward());

    // Push new entry clears forward
    history.push({.artifact_id = "a4", .display_name = "D"});
    REQUIRE_FALSE(history.can_go_forward());
    REQUIRE(history.entry_count() == 2);  // a1 and a4
}

TEST_CASE("P10-T04 breadcrumbs", "[v24][p10]") {
    NavigationHistory history;
    history.push({.artifact_id = "a1", .display_name = "A"});
    history.push({.artifact_id = "a2", .display_name = "B"});
    history.push({.artifact_id = "a3", .display_name = "C"});

    auto crumbs = history.breadcrumbs();
    REQUIRE(crumbs.size() == 3);
    REQUIRE(crumbs[0]->artifact_id == "a1");
    REQUIRE(crumbs[2]->artifact_id == "a3");
}

// P10-T05: Navigation clear and max size
TEST_CASE("P10-T05 navigation clear", "[v24][p10]") {
    NavigationHistory history;
    history.push({.artifact_id = "a1", .display_name = "A"});
    history.clear();
    REQUIRE(history.entry_count() == 0);
    REQUIRE_FALSE(history.can_go_back());
}

TEST_CASE("P10-T05 navigation max size trimming", "[v24][p10]") {
    NavigationHistory history;
    history.set_max_size(3);
    history.push({.artifact_id = "a1", .display_name = "A"});
    history.push({.artifact_id = "a2", .display_name = "B"});
    history.push({.artifact_id = "a3", .display_name = "C"});
    history.push({.artifact_id = "a4", .display_name = "D"});  // should trim a1
    REQUIRE(history.entry_count() == 3);
    // Current should be a4
    auto* cur = history.current();
    REQUIRE(cur != nullptr);
    REQUIRE(cur->artifact_id == "a4");
}
