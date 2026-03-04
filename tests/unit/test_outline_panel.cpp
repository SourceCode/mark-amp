#include "../../src/core/Config.h"
#include "../../src/core/EventBus.h"
#include "../../src/core/Events.h"
#include "../../src/core/Outline.h"
#include "../../src/core/OutlineService.h"
#include "../../src/ui/OutlineIconProvider.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// OutlineBuilder Tests
// ============================================================================

TEST_CASE("OutlineBuilder builds hierarchy from flat headings", "[outline][builder]")
{
    OutlineBuilder builder;

    SECTION("Single H1 heading")
    {
        std::vector<std::pair<std::string, int>> headings = {{"Introduction", 1}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 1);
        REQUIRE(outline.roots.size() == 1);
        REQUIRE(outline.roots[0].text == "Introduction");
        REQUIRE(outline.roots[0].level == 1);
        REQUIRE(outline.level_counts[0] == 1); // H1
    }

    SECTION("Multiple H1 headings")
    {
        std::vector<std::pair<std::string, int>> headings = {
            {"Chapter 1", 1}, {"Chapter 2", 1}, {"Chapter 3", 1}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 3);
        REQUIRE(outline.roots.size() == 3);
        REQUIRE(outline.level_counts[0] == 3);
    }

    SECTION("Nested headings H1 > H2 > H3")
    {
        std::vector<std::pair<std::string, int>> headings = {
            {"Chapter", 1}, {"Section", 2}, {"Subsection", 3}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 3);
        REQUIRE(outline.roots.size() == 1);
        REQUIRE(outline.roots[0].children.size() == 1);
        REQUIRE(outline.roots[0].children[0].text == "Section");
        REQUIRE(outline.roots[0].children[0].children.size() == 1);
        REQUIRE(outline.roots[0].children[0].children[0].text == "Subsection");
    }

    SECTION("Empty document")
    {
        std::vector<std::pair<std::string, int>> headings = {};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.is_empty());
        REQUIRE(outline.total_heading_count == 0);
    }

    SECTION("All same level (flat)")
    {
        std::vector<std::pair<std::string, int>> headings = {{"A", 2}, {"B", 2}, {"C", 2}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 3);
        REQUIRE(outline.level_counts[1] == 3); // H2
    }

    SECTION("Deeply nested H1>H2>H3>H4>H5>H6")
    {
        std::vector<std::pair<std::string, int>> headings = {
            {"L1", 1}, {"L2", 2}, {"L3", 3}, {"L4", 4}, {"L5", 5}, {"L6", 6}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 6);
        for (int level = 0; level < 6; ++level)
        {
            REQUIRE(outline.level_counts[level] == 1);
        }
    }

    SECTION("Level counts array populated correctly")
    {
        std::vector<std::pair<std::string, int>> headings = {
            {"A", 1}, {"B", 2}, {"C", 2}, {"D", 3}, {"E", 1}, {"F", 2}};
        auto outline = builder.build_from_headings(headings);
        REQUIRE(outline.total_heading_count == 6);
        REQUIRE(outline.level_counts[0] == 2); // H1
        REQUIRE(outline.level_counts[1] == 3); // H2
        REQUIRE(outline.level_counts[2] == 1); // H3
    }
}

// ============================================================================
// OutlineNode Tests
// ============================================================================

TEST_CASE("OutlineNode sub_heading_count", "[outline][node]")
{
    OutlineNode node;
    node.text = "Parent";
    REQUIRE(node.sub_heading_count() == 0);

    OutlineNode child1;
    child1.text = "Child1";
    OutlineNode child2;
    child2.text = "Child2";
    node.children.push_back(child1);
    node.children.push_back(child2);
    REQUIRE(node.sub_heading_count() == 2);
}

// ============================================================================
// DocumentOutline Tests
// ============================================================================

TEST_CASE("DocumentOutline is_empty", "[outline][document]")
{
    DocumentOutline outline;
    REQUIRE(outline.is_empty());

    outline.total_heading_count = 5;
    REQUIRE_FALSE(outline.is_empty());
}

// ============================================================================
// OutlineService Tests
// ============================================================================

TEST_CASE("OutlineService basic operations", "[outline][service]")
{
    EventBus event_bus;
    Config config;
    OutlineService service(event_bus, config);

    SECTION("Empty outline for unknown document")
    {
        auto outline = service.get_outline("unknown_doc");
        REQUIRE(outline.is_empty());
    }

    SECTION("Flat outline for unknown document")
    {
        auto entries = service.get_flat_outline("unknown_doc");
        REQUIRE(entries.empty());
    }

    SECTION("Search headings in empty outline")
    {
        auto results = service.search_headings("unknown_doc", "test");
        REQUIRE(results.empty());
    }

    SECTION("Invalidation marks outline stale")
    {
        service.invalidate("doc1");
        auto outline = service.get_outline("doc1");
        REQUIRE(outline.is_empty());
    }
}

TEST_CASE("OutlineService content changed rebuilds outline", "[outline][service]")
{
    EventBus event_bus;
    Config config;
    OutlineService service(event_bus, config);

    std::string markdown = "# Title\n## Section 1\n### Detail\n## Section 2\n";
    service.on_content_changed(markdown, "test_doc");

    auto outline = service.get_outline("test_doc");
    REQUIRE(outline.root_id == "test_doc");
}

TEST_CASE("OutlineService breadcrumb path", "[outline][service][breadcrumb]")
{
    EventBus event_bus;
    Config config;
    OutlineService service(event_bus, config);

    auto path = service.get_breadcrumb_path("unknown_doc", 5);
    REQUIRE(path.empty());
}

TEST_CASE("OutlineService heading_at_line", "[outline][service]")
{
    EventBus event_bus;
    Config config;
    OutlineService service(event_bus, config);

    auto heading = service.get_heading_at_line("unknown_doc", 5);
    REQUIRE_FALSE(heading.has_value());
}

// ============================================================================
// OutlineIconProvider Tests
// ============================================================================

TEST_CASE("OutlineIconProvider icon_for_heading_level", "[outline][icons]")
{
    REQUIRE(icon_for_heading_level(1) == OutlineIconType::kH1);
    REQUIRE(icon_for_heading_level(2) == OutlineIconType::kH2);
    REQUIRE(icon_for_heading_level(3) == OutlineIconType::kH3);
    REQUIRE(icon_for_heading_level(4) == OutlineIconType::kH4);
    REQUIRE(icon_for_heading_level(5) == OutlineIconType::kH5);
    REQUIRE(icon_for_heading_level(6) == OutlineIconType::kH6);
    REQUIRE(icon_for_heading_level(0) == OutlineIconType::kUnknown);
    REQUIRE(icon_for_heading_level(7) == OutlineIconType::kUnknown);
}

TEST_CASE("OutlineIconProvider outline_icon_char", "[outline][icons]")
{
    REQUIRE(outline_icon_char(OutlineIconType::kH1) == "H1");
    REQUIRE(outline_icon_char(OutlineIconType::kH2) == "H2");
    REQUIRE(outline_icon_char(OutlineIconType::kFunction) == "f()");
    REQUIRE(outline_icon_char(OutlineIconType::kClass) == "C");
    REQUIRE(outline_icon_char(OutlineIconType::kVariable) == "x");
    REQUIRE(outline_icon_char(OutlineIconType::kNamespace) == "N");
    REQUIRE(outline_icon_char(OutlineIconType::kInterface) == "I");
    REQUIRE(outline_icon_char(OutlineIconType::kEnum) == "E");
    REQUIRE(outline_icon_char(OutlineIconType::kConstant) == "K");
    REQUIRE(outline_icon_char(OutlineIconType::kProperty) == "P");
    REQUIRE(outline_icon_char(OutlineIconType::kUnknown) == "?");
}

TEST_CASE("OutlineIconProvider icon_for_symbol_kind", "[outline][icons]")
{
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kFunction) ==
            OutlineIconType::kFunction);
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kClass) == OutlineIconType::kClass);
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kVariable) ==
            OutlineIconType::kVariable);
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kNamespace) ==
            OutlineIconType::kNamespace);
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kInterface) ==
            OutlineIconType::kInterface);
    REQUIRE(icon_for_symbol_kind(markamp::core::SymbolKind::kHeading) == OutlineIconType::kH1);
}

TEST_CASE("OutlineIconProvider outline_label", "[outline][icons]")
{
    REQUIRE(outline_label(OutlineIconType::kH1, "Introduction") == "H1 Introduction");
    REQUIRE(outline_label(OutlineIconType::kFunction, "main") == "f() main");
}
