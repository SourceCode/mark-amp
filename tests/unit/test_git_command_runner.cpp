/// @file test_git_command_runner.cpp
/// @brief V4 Phase 35 \u2013 GitCommandRunner tests.

#include "core/GitCommandRunner.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::core;

TEST_CASE("GitCommandRunner parses git blame output correctly", "[git][git_command]")
{
    std::string blameOutput = R"(1a2b3c4d00000000000000000000000000000000 1 1 1
author John Doe
author-mail <john@example.com>
author-time 1698393600
author-tz +0000
committer John Doe
committer-mail <john@example.com>
committer-time 1698393600
committer-tz +0000
summary Line 1 commit
boundary
filename test.txt
	Line 1 content
^5e6f7g800000000000000000000000000000000 2 2 1
author Jane Smith
author-mail <jane@example.com>
author-time 1698307200
author-tz +0000
committer Jane Smith
committer-mail <jane@example.com>
committer-time 1698307200
committer-tz +0000
summary Line 2 commit
previous 4d5e6f7g00000000000000000000000000000000 test.txt
filename test.txt
	Line 2 content
9h0i1j2k00000000000000000000000000000000 3 3 1
author John Doe
author-mail <john@example.com>
author-time 1698393900
author-tz +0000
committer John Doe
committer-mail <john@example.com>
committer-time 1698393900
committer-tz +0000
summary Line 3 commit
filename test.txt
	Line 3 content)";

    auto annotations = GitCommandRunner::ParseBlameOutput(blameOutput);

    REQUIRE(annotations.size() == 3);

    REQUIRE(annotations[0].commit_hash == "1a2b3c4d00000000000000000000000000000000");
    REQUIRE(annotations[0].author == "John Doe");
    REQUIRE(annotations[0].date == "1698393600");
    REQUIRE(annotations[0].original_line == 1);

    REQUIRE(annotations[1].commit_hash ==
            "^5e6f7g800000000000000000000000000000000"); // Keep the caret if it's there
    REQUIRE(annotations[1].author == "Jane Smith");
    REQUIRE(annotations[1].date == "1698307200");
    REQUIRE(annotations[1].original_line == 2);

    REQUIRE(annotations[2].commit_hash == "9h0i1j2k00000000000000000000000000000000");
    REQUIRE(annotations[2].author == "John Doe");
    REQUIRE(annotations[2].date == "1698393900");
    REQUIRE(annotations[2].original_line == 3);
}

TEST_CASE("GitCommandRunner handles empty blame output", "[git][git_command]")
{
    std::string blameOutput = "";
    auto annotations = GitCommandRunner::ParseBlameOutput(blameOutput);
    REQUIRE(annotations.empty());
}

TEST_CASE("GitCommandRunner handles malformed blame output gracefully", "[git][git_command]")
{
    std::string blameOutput = "This is not a git blame output\nAnother malformed line";
    auto annotations = GitCommandRunner::ParseBlameOutput(blameOutput);
    // Depending on the regex, it might ignore these or return empty annotations
    REQUIRE(annotations.empty());
}
