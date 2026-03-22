/// @file test_v25_p10_workspace.cpp
/// @brief V25 Phase 10: Workspace continuity tests.
#include <catch2/catch_test_macros.hpp>
#include "core/V25ReleaseServices.h"

using namespace markamp::core;

TEST_CASE("V25 P10: Workspace entry validation", "[v25][p10]")
{
    WorkspaceContinuityValidator validator;
    WorkspaceRestoreEntry e;
    e.entry_id = "doc1";
    e.artifact_path = "/path/doc1.md";
    e.exists_on_disk = true;
    validator.add_entry(e);

    auto results = validator.validate_all();
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].is_valid);
}

TEST_CASE("V25 P10: Invalid entries detected", "[v25][p10]")
{
    WorkspaceContinuityValidator validator;
    WorkspaceRestoreEntry e;
    e.entry_id = "missing";
    e.exists_on_disk = false;
    validator.add_entry(e);
    validator.validate_all();
    REQUIRE(validator.invalid_entries().size() == 1);
}

TEST_CASE("V25 P10: Entry count", "[v25][p10]")
{
    WorkspaceContinuityValidator validator;
    REQUIRE(validator.entry_count() == 0);
    WorkspaceRestoreEntry e;
    e.entry_id = "doc1";
    validator.add_entry(e);
    REQUIRE(validator.entry_count() == 1);
}
