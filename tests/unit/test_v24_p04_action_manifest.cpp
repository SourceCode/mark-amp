/// @file test_v24_p04_action_manifest.cpp
/// @brief V24 Phase 04 tests: ActionManifest, shell command authority.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ActionManifest.h"
#include "../../src/core/ContextKeyService.h"

using namespace markamp::core;

// P04-T01: Single action manifest
TEST_CASE("P04-T01 register and execute action", "[v24][p04]") {
    ActionManifest manifest;
    bool executed = false;
    ManifestAction a;
    a.action_id = "file.new";
    a.label = "New File";
    a.category = ActionCategory::kFile;
    a.handler = [&]() { executed = true; };
    REQUIRE(manifest.register_action(std::move(a)));

    auto result = manifest.execute("file.new");
    REQUIRE(result.ok());
    REQUIRE(executed);
}

TEST_CASE("P04-T01 action not found", "[v24][p04]") {
    ActionManifest manifest;
    auto result = manifest.execute("nonexistent");
    REQUIRE_FALSE(result.ok());
    REQUIRE(result.error_message.find("not found") != std::string::npos);
}

TEST_CASE("P04-T01 duplicate registration rejected", "[v24][p04]") {
    ActionManifest manifest;
    ManifestAction a;
    a.action_id = "file.new";
    a.handler = [](){};
    REQUIRE(manifest.register_action(std::move(a)));

    ManifestAction b;
    b.action_id = "file.new";
    b.handler = [](){};
    REQUIRE_FALSE(manifest.register_action(std::move(b)));
}

// P04-T02: Enablement via context keys
TEST_CASE("P04-T02 enablement predicate blocks execution", "[v24][p04]") {
    ActionManifest manifest;
    ManifestAction a;
    a.action_id = "edit.undo";
    a.label = "Undo";
    a.handler = [](){};
    a.enablement = []() { return false; };  // Always disabled
    REQUIRE(manifest.register_action(std::move(a)));

    REQUIRE_FALSE(manifest.is_enabled("edit.undo"));
    auto result = manifest.execute("edit.undo");
    REQUIRE_FALSE(result.ok());
}

TEST_CASE("P04-T02 enabled action executes", "[v24][p04]") {
    ActionManifest manifest;
    bool ran = false;
    ManifestAction a;
    a.action_id = "edit.redo";
    a.handler = [&]() { ran = true; };
    a.enablement = []() { return true; };
    REQUIRE(manifest.register_action(std::move(a)));

    REQUIRE(manifest.is_enabled("edit.redo"));
    REQUIRE(manifest.execute("edit.redo").ok());
    REQUIRE(ran);
}

// P04-T03: Category queries
TEST_CASE("P04-T03 actions by category", "[v24][p04]") {
    ActionManifest manifest;
    ManifestAction a1;
    a1.action_id = "file.new"; a1.category = ActionCategory::kFile; a1.handler = [](){};
    ManifestAction a2;
    a2.action_id = "file.save"; a2.category = ActionCategory::kFile; a2.handler = [](){};
    ManifestAction a3;
    a3.action_id = "edit.cut"; a3.category = ActionCategory::kEdit; a3.handler = [](){};
    REQUIRE(manifest.register_action(std::move(a1)));
    REQUIRE(manifest.register_action(std::move(a2)));
    REQUIRE(manifest.register_action(std::move(a3)));

    auto file_actions = manifest.actions_by_category(ActionCategory::kFile);
    REQUIRE(file_actions.size() == 2);
    auto edit_actions = manifest.actions_by_category(ActionCategory::kEdit);
    REQUIRE(edit_actions.size() == 1);
}

// P04-T04: Context key evaluation
TEST_CASE("P04-T04 context key service basic ops", "[v24][p04]") {
    ContextKeyService ctx;
    ctx.set_context("editorFocus", std::string("true"));
    REQUIRE(ctx.get_string("editorFocus") == "true");
    REQUIRE(ctx.get_string("missing").empty());
}

// P04-T05: Action manifest lookup and clear
TEST_CASE("P04-T05 get_action and all_actions", "[v24][p04]") {
    ActionManifest manifest;
    ManifestAction a1;
    a1.action_id = "view.zoom-in"; a1.label = "Zoom In"; a1.handler = [](){};
    ManifestAction a2;
    a2.action_id = "view.zoom-out"; a2.label = "Zoom Out"; a2.handler = [](){};
    REQUIRE(manifest.register_action(std::move(a1)));
    REQUIRE(manifest.register_action(std::move(a2)));

    auto* a = manifest.get_action("view.zoom-in");
    REQUIRE(a != nullptr);
    REQUIRE(a->label == "Zoom In");

    REQUIRE(manifest.all_actions().size() == 2);
    REQUIRE(manifest.action_count() == 2);

    manifest.clear();
    REQUIRE(manifest.action_count() == 0);
}

TEST_CASE("P04-T05 action without handler", "[v24][p04]") {
    ActionManifest manifest;
    ManifestAction a;
    a.action_id = "stub.action";
    REQUIRE(manifest.register_action(std::move(a)));  // no handler
    auto result = manifest.execute("stub.action");
    REQUIRE_FALSE(result.ok());
    REQUIRE(result.error_message.find("no handler") != std::string::npos);
}
