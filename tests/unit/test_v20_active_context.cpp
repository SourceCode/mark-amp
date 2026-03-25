/// @file test_v20_active_context.cpp
/// @brief V20 Phase 06 – ActiveContextService unit tests.

#include "core/ActiveContextService.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ActiveContext: construction", "[v20][active-context]")
{
    ActiveContextService service;
    REQUIRE(service.update_count() == 0);
    REQUIRE_FALSE(service.context().has_artifact());
}

TEST_CASE("ActiveContext: set context", "[v20][active-context]")
{
    ActiveContextService service;

    ActiveContext ctx;
    ctx.active_artifact = ArtifactId{"art-1"};
    ctx.active_surface = ActiveSurfaceKind::kEditor;
    ctx.focus_target = "editor-panel";
    ctx.selection_count = 5;
    ctx.active_language = "markdown";
    ctx.is_editing = true;

    service.set_context(ctx);

    REQUIRE(service.context().has_artifact());
    REQUIRE(service.context().is_editor());
    REQUIRE(service.update_count() == 1);

    // Context keys auto-populated
    REQUIRE(service.get_context_key("activeSurface") == "editor");
    REQUIRE(service.get_context_key("activeLanguage") == "markdown");
    REQUIRE(service.get_context_key("isEditing") == "true");
}

TEST_CASE("ActiveContext: context keys", "[v20][active-context]")
{
    ActiveContextService service;

    service.set_context_key("customKey", "customValue");
    REQUIRE(service.get_context_key("customKey") == "customValue");

    service.clear_context_key("customKey");
    REQUIRE_FALSE(service.get_context_key("customKey").has_value());
}

TEST_CASE("ActiveContext: all context keys", "[v20][active-context]")
{
    ActiveContextService service;
    service.set_context_key("a", "1");
    service.set_context_key("b", "2");

    auto keys = service.all_context_keys();
    REQUIRE(keys.size() == 2);
}

TEST_CASE("ActiveContext: evaluate_when", "[v20][active-context]")
{
    ActiveContextService service;
    service.set_context_key("activeSurface", "canvas");
    service.set_context_key("isEditing", "true");

    REQUIRE(service.evaluate_when("activeSurface == canvas"));
    REQUIRE_FALSE(service.evaluate_when("activeSurface == editor"));
    REQUIRE(service.evaluate_when("isEditing"));
    REQUIRE(service.evaluate_when("")); // empty = always true
}

TEST_CASE("Phase 06 events: ActiveContextChangedEvent", "[v20][context-events]")
{
    events::ActiveContextChangedEvent evt;
    evt.surface = "canvas";
    evt.selection_count = 3;
    REQUIRE(evt.selection_count == 3);
}

TEST_CASE("Phase 06 events: ContextKeyChangedEvent", "[v20][context-events]")
{
    events::ContextKeyChangedEvent evt;
    evt.key = "activeSurface";
    evt.value = "editor";
    REQUIRE(evt.key == "activeSurface");
}
