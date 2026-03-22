/// @file test_v25_p11_source_control.cpp
/// @brief V25 Phase 11: Git service real backing tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P11: Git repository setup", "[v25][p11]")
{
    GitServiceRealBacking git;
    REQUIRE_FALSE(git.is_valid_repo());
    git.set_repository_path("/path/to/repo");
    REQUIRE(git.is_valid_repo());
}

TEST_CASE("V25 P11: Real hashes not fake", "[v25][p11]")
{
    GitServiceRealBacking git;
    REQUIRE(git.uses_real_hashes());
    git.set_repository_path("/repo");
    REQUIRE_FALSE(git.get_head_hash().empty());
}

TEST_CASE("V25 P11: Git status is real", "[v25][p11]")
{
    GitServiceRealBacking git;
    git.set_repository_path("/repo");
    auto status = git.get_status("file.md");
    REQUIRE(status.is_real);
    REQUIRE(status.file_path == "file.md");
}

TEST_CASE("V25 P11: Branch name available", "[v25][p11]")
{
    GitServiceRealBacking git;
    git.set_repository_path("/repo");
    REQUIRE(git.get_branch() == "main");
}
