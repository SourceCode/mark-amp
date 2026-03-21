/// @file test_v20_icon_semantic.cpp
/// @brief V20 Phase 09 – IconSemanticMapper unit tests.

#include "core/IconSemanticMapper.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("IconSemantic: defaults registered", "[v20][icon-semantic]")
{
    EventBus bus;
    IconSemanticMapper mapper(bus);

    REQUIRE(mapper.icon_count() >= 17);
    REQUIRE(mapper.residue_count() == 0);
}

TEST_CASE("IconSemantic: icon lookup", "[v20][icon-semantic]")
{
    EventBus bus;
    IconSemanticMapper mapper(bus);

    auto* new_file = mapper.icon("file.new");
    REQUIRE(new_file != nullptr);
    REQUIRE(new_file->icon_name == "codicon-new-file");
    REQUIRE(new_file->has_label());
    REQUIRE(new_file->accessibility_label == "New File");

    REQUIRE(mapper.icon("nonexistent") == nullptr);
}

TEST_CASE("IconSemantic: all icons have labels", "[v20][icon-semantic]")
{
    EventBus bus;
    IconSemanticMapper mapper(bus);

    auto unlabeled = mapper.unlabeled_icons();
    REQUIRE(unlabeled.empty());
}

TEST_CASE("IconSemantic: residue tracking", "[v20][icon-semantic]")
{
    EventBus bus;
    IconSemanticMapper mapper(bus);

    IconResidueEntry residue;
    residue.surface = "canvas-toolbar";
    residue.old_icon = "▶";
    residue.description = "Unicode play button placeholder";
    mapper.report_residue(residue);

    REQUIRE(mapper.residue_count() == 1);
    REQUIRE(mapper.all_residue()[0].old_icon == "▶");
}

TEST_CASE("IconSemantic: custom icon registration", "[v20][icon-semantic]")
{
    EventBus bus;
    IconSemanticMapper mapper(bus);

    int initial = mapper.icon_count();

    SemanticIcon custom;
    custom.action_id = "custom.action";
    custom.icon_name = "codicon-custom";
    custom.accessibility_label = "Custom Action";
    mapper.register_icon(custom);

    REQUIRE(mapper.icon_count() == initial + 1);
    REQUIRE(mapper.icon("custom.action")->icon_name == "codicon-custom");
}
