/// @file test_v24_p11_source_control.cpp
/// @brief V24 Phase 11 tests: Source control, Git structures, SCM readiness.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/GitService.h"

using namespace markamp::core;

// P11-T01: Git file status enum
TEST_CASE("P11-T01 git file status values", "[v24][p11]") {
    REQUIRE(GitFileStatus::kUnmodified != GitFileStatus::kModified);
    REQUIRE(GitFileStatus::kAdded != GitFileStatus::kDeleted);
    REQUIRE(GitFileStatus::kRenamed != GitFileStatus::kUntracked);
    REQUIRE(GitFileStatus::kIgnored != GitFileStatus::kConflicted);
}

TEST_CASE("P11-T01 git file entry defaults", "[v24][p11]") {
    GitFileEntry entry;
    REQUIRE(entry.status == GitFileStatus::kUnmodified);
    REQUIRE(entry.staged_status == GitFileStatus::kUnmodified);
    REQUIRE_FALSE(entry.is_staged);
}

// P11-T02: Diff structures
TEST_CASE("P11-T02 diff hunk structure", "[v24][p11]") {
    DiffHunk hunk;
    hunk.old_start = 10;
    hunk.old_count = 5;
    hunk.new_start = 10;
    hunk.new_count = 8;
    hunk.lines = {"+added line 1", "+added line 2", "+added line 3", " unchanged", "-removed"};
    REQUIRE(hunk.lines.size() == 5);
    REQUIRE(hunk.old_count == 5);
    REQUIRE(hunk.new_count == 8);
}

TEST_CASE("P11-T02 file diff aggregation", "[v24][p11]") {
    FileDiff diff;
    diff.file_path = "/src/main.cpp";
    diff.additions = 15;
    diff.deletions = 3;
    DiffHunk h;
    h.old_start = 1; h.new_start = 1;
    diff.hunks.push_back(h);
    REQUIRE(diff.additions == 15);
    REQUIRE(diff.deletions == 3);
    REQUIRE(diff.hunks.size() == 1);
}

// P11-T03: Commit structures
TEST_CASE("P11-T03 git commit structure", "[v24][p11]") {
    GitCommit commit;
    commit.hash = "abc123def456";
    commit.short_hash = "abc123d";
    commit.author = "Dev";
    commit.email = "dev@example.com";
    commit.message = "Fix bug #42";
    commit.changed_files = {"src/main.cpp", "tests/test.cpp"};
    REQUIRE(commit.changed_files.size() == 2);
    REQUIRE(commit.message == "Fix bug #42");
}

TEST_CASE("P11-T03 git branch structure", "[v24][p11]") {
    GitBranch branch;
    branch.name = "feature/new-ui";
    branch.is_current = true;
    branch.ahead = 3;
    branch.behind = 1;
    REQUIRE(branch.is_current);
    REQUIRE(branch.ahead == 3);
}

// P11-T04: SCM state
TEST_CASE("P11-T04 git file entry staged state", "[v24][p11]") {
    GitFileEntry entry;
    entry.file_path = "README.md";
    entry.status = GitFileStatus::kModified;
    entry.is_staged = true;
    entry.staged_status = GitFileStatus::kModified;
    REQUIRE(entry.is_staged);
    REQUIRE(entry.status == GitFileStatus::kModified);
}

TEST_CASE("P11-T04 git branch remote tracking", "[v24][p11]") {
    GitBranch remote;
    remote.name = "origin/main";
    remote.is_remote = true;
    remote.upstream = "main";
    REQUIRE(remote.is_remote);
    REQUIRE(remote.upstream == "main");
}

// P11-T05: Regression
TEST_CASE("P11-T05 diff hunk line prefix convention", "[v24][p11]") {
    DiffHunk hunk;
    hunk.lines = {"+new", "-old", " context"};
    REQUIRE(hunk.lines[0][0] == '+');
    REQUIRE(hunk.lines[1][0] == '-');
    REQUIRE(hunk.lines[2][0] == ' ');
}
