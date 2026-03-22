/// @file test_v25_p04_shell_actions.cpp
/// @brief V25 Phase 04: Action manifest bootstrap tests.
#include <catch2/catch_test_macros.hpp>
#include "core/ActionManifestBootstrap.h"

using namespace markamp::core;

TEST_CASE("V25 P04: Bootstrap populates release actions", "[v25][p04]")
{
    ActionManifest manifest;
    bootstrap_release_actions(manifest);
    REQUIRE(manifest.action_count() >= 40);
}

TEST_CASE("V25 P04: Release action count constant", "[v25][p04]")
{
    REQUIRE(release_action_count() == 42);
}

TEST_CASE("V25 P04: File actions are registered", "[v25][p04]")
{
    ActionManifest manifest;
    bootstrap_release_actions(manifest);

    auto* action = manifest.get_action("file.new");
    REQUIRE(action != nullptr);
    REQUIRE(action->label == "New File");
}

TEST_CASE("V25 P04: All categories are covered", "[v25][p04]")
{
    ActionManifest manifest;
    bootstrap_release_actions(manifest);

    REQUIRE(manifest.get_action("edit.undo") != nullptr);
    REQUIRE(manifest.get_action("view.sidebar") != nullptr);
    REQUIRE(manifest.get_action("navigate.back") != nullptr);
    REQUIRE(manifest.get_action("search.find") != nullptr);
    REQUIRE(manifest.get_action("help.about") != nullptr);
}
