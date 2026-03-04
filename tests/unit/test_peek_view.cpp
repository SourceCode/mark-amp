/// @file test_peek_view.cpp
/// @brief Phase 47 — Unit tests for Peek View System.

#include "core/PeekProvider.h"
#include "ui/PeekEditorHost.h"
#include "ui/PeekResultList.h"
#include "ui/PeekView.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// PeekProviderRegistry
// ============================================================================

TEST_CASE("PeekProviderRegistry: maps file types to providers", "[peek_view]")
{
    PeekProviderRegistry registry;
    auto md_provider = std::make_shared<MarkdownPeekProvider>();
    auto cpp_provider = std::make_shared<CppPeekProvider>();

    registry.register_provider(md_provider);
    registry.register_provider(cpp_provider);

    REQUIRE(registry.provider_for(".md") != nullptr);
    REQUIRE(registry.provider_for(".cpp") != nullptr);
    REQUIRE(registry.provider_for(".py") == nullptr);
}

TEST_CASE("MarkdownPeekProvider: resolves registered files", "[peek_view]")
{
    MarkdownPeekProvider provider;
    provider.register_file("/notes/readme.md", "README");
    provider.register_file("/notes/setup.md", "Setup Guide");

    auto defs = provider.find_definitions("test.md", 0, 0);
    REQUIRE(defs.size() == 2);
}

TEST_CASE("MarkdownPeekProvider: returns backlinks as references", "[peek_view]")
{
    MarkdownPeekProvider provider;
    provider.add_backlink("/target.md",
                          {.file_path = "/source.md",
                           .line = 5,
                           .preview_text = "See [[target]]",
                           .symbol_name = "target"});

    auto refs = provider.find_references("/target.md", 0, 0);
    REQUIRE(refs.size() == 1);
    REQUIRE(refs[0].file_path == "/source.md");
}

// ============================================================================
// PeekView model
// ============================================================================

TEST_CASE("PeekView: navigation stack with back", "[peek_view]")
{
    PeekViewModel model;
    model.open(PeekCommand::Definition, "test.cpp", 10, 5, {{.file_path = "def.cpp", .line = 20}});

    REQUIRE(model.state() == PeekViewState::Open);
    REQUIRE(model.stack_depth() == 1);

    model.push(PeekCommand::References,
               "def.cpp",
               20,
               0,
               {{.file_path = "ref1.cpp", .line = 30}, {.file_path = "ref2.cpp", .line = 40}});

    REQUIRE(model.stack_depth() == 2);
    REQUIRE(model.result_count() == 2);

    model.pop();
    REQUIRE(model.stack_depth() == 1);
    REQUIRE(model.result_count() == 1);
}

TEST_CASE("PeekView: resize respects min/max bounds", "[peek_view]")
{
    PeekViewModel model;
    model.set_height(50); // Below min
    REQUIRE(model.height() == PeekViewModel::kMinHeight);

    model.set_height(1000); // Above max
    REQUIRE(model.height() == PeekViewModel::kMaxHeight);

    model.set_height(300);
    REQUIRE(model.height() == 300);
}

TEST_CASE("PeekResultList: groups results by file", "[peek_view]")
{
    PeekResultListModel list;
    list.set_results({
        {.file_path = "/a.cpp", .line = 10, .symbol_name = "foo"},
        {.file_path = "/a.cpp", .line = 20, .symbol_name = "bar"},
        {.file_path = "/b.cpp", .line = 5, .symbol_name = "baz"},
    });

    REQUIRE(list.group_count() == 2);
    REQUIRE(list.groups()[0].result_indices.size() == 2);
    REQUIRE(list.groups()[1].result_indices.size() == 1);
}

TEST_CASE("CppPeekProvider: finds registered definitions", "[peek_view]")
{
    CppPeekProvider provider;
    provider.register_symbol("MyClass",
                             {.file_path = "/src/MyClass.h", .line = 10, .symbol_name = "MyClass"});

    auto defs = provider.find_definitions("main.cpp", 5, 10);
    REQUIRE(defs.size() == 1);
    REQUIRE(defs[0].symbol_name == "MyClass");
}

TEST_CASE("PeekHierarchyNode: descendant count", "[peek_view]")
{
    PeekHierarchyNode root{.name = "root"};
    root.children.push_back({.name = "child1"});
    root.children.push_back({.name = "child2"});
    root.children[0].children.push_back({.name = "grandchild"});

    REQUIRE(root.descendant_count() == 3);
}

TEST_CASE("PeekView: close restores state", "[peek_view]")
{
    PeekViewModel model;
    model.open(PeekCommand::Definition, "test.cpp", 10, 5, {{.file_path = "def.cpp", .line = 20}});

    REQUIRE(model.state() == PeekViewState::Open);
    model.close();
    REQUIRE(model.state() == PeekViewState::Closed);
    REQUIRE(model.stack_depth() == 0);
}
