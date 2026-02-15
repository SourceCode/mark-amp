/// @file test_git_service.cpp
/// @brief V4 Phase 35 – GitService tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/GitService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Test fixture
// ============================================================================

struct GitFixture
{
    EventBus event_bus;
    Config config;
    GitService git{event_bus, config};
};

// ============================================================================
// Repository management
// ============================================================================

TEST_CASE("GitService opens repository", "[phase35][git]")
{
    GitFixture ctx;
    REQUIRE_FALSE(ctx.git.is_repository());

    auto result = ctx.git.open_repository("/tmp/testrepo");
    REQUIRE(result.has_value());
    REQUIRE(ctx.git.is_repository());
    REQUIRE(ctx.git.repository_path() == "/tmp/testrepo");
}

TEST_CASE("GitService rejects empty repository path", "[phase35][git]")
{
    GitFixture ctx;
    auto result = ctx.git.open_repository("");
    REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// File status
// ============================================================================

TEST_CASE("GitService tracks file status", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    ctx.git.test_add_file("readme.md", "# Hello");

    auto entries = ctx.git.status();
    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].file_path == "readme.md");
    REQUIRE(entries[0].status == GitFileStatus::kUntracked);
}

// ============================================================================
// Diff
// ============================================================================

TEST_CASE("GitService generates diff for modified file", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    ctx.git.test_add_file("file.txt", "line1\nline2");
    ctx.git.stage_file("file.txt");
    ctx.git.commit("initial");

    ctx.git.test_modify_file("file.txt", "line1\nmodified");

    auto diff = ctx.git.diff_file("file.txt");
    REQUIRE(diff.file_path == "file.txt");
    REQUIRE_FALSE(diff.hunks.empty());
    REQUIRE(diff.additions > 0);
    REQUIRE(diff.deletions > 0);
}

// ============================================================================
// Staging
// ============================================================================

TEST_CASE("GitService stages and unstages files", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    ctx.git.test_add_file("file.txt", "content");

    REQUIRE(ctx.git.stage_file("file.txt").has_value());
    auto entries = ctx.git.status();
    REQUIRE(entries[0].is_staged);

    REQUIRE(ctx.git.unstage_file("file.txt").has_value());
    entries = ctx.git.status();
    REQUIRE_FALSE(entries[0].is_staged);
}

TEST_CASE("GitService fails to stage non-existent file", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    auto result = ctx.git.stage_file("nonexistent.txt");
    REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// Commit
// ============================================================================

TEST_CASE("GitService creates commit from staged files", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    ctx.git.test_add_file("file.txt", "content");
    ctx.git.stage_file("file.txt");

    auto result = ctx.git.commit("Initial commit");
    REQUIRE(result.has_value());

    auto commits = ctx.git.log();
    REQUIRE(commits.size() == 1);
    REQUIRE(commits[0].message == "Initial commit");
    REQUIRE_FALSE(commits[0].hash.empty());
}

TEST_CASE("GitService rejects empty commit message", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    auto result = ctx.git.commit("");
    REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// Branches
// ============================================================================

TEST_CASE("GitService manages branches", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");

    REQUIRE(ctx.git.current_branch() == "main");

    auto create = ctx.git.create_branch("feature");
    REQUIRE(create.has_value());

    auto branches = ctx.git.branches();
    REQUIRE(branches.size() == 2);

    auto checkout = ctx.git.checkout_branch("feature");
    REQUIRE(checkout.has_value());
    REQUIRE(ctx.git.current_branch() == "feature");
}

// ============================================================================
// Discard
// ============================================================================

TEST_CASE("GitService discards changes", "[phase35][git]")
{
    GitFixture ctx;
    ctx.git.open_repository("/tmp/repo");
    ctx.git.test_add_file("temp.txt", "temp");
    ctx.git.stage_file("temp.txt");
    ctx.git.commit("add temp");

    ctx.git.test_modify_file("temp.txt", "modified");
    REQUIRE(ctx.git.file_status("temp.txt") == GitFileStatus::kModified);

    auto result = ctx.git.discard_file("temp.txt");
    REQUIRE(result.has_value());
    REQUIRE(ctx.git.file_status("temp.txt") == GitFileStatus::kUnmodified);
}
