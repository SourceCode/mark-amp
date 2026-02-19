// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/PdfObjectModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Import mode and page range", "[pdf][import]")
{
    PdfObjectModel model;
    model.set_import_mode(PdfImportMode::kPageRange);
    model.set_page_range(3, 7);
    REQUIRE(model.import_mode() == PdfImportMode::kPageRange);
    REQUIRE(model.range_start() == 3);
    REQUIRE(model.range_end() == 7);
}

TEST_CASE("Page range validation", "[pdf][import]")
{
    PdfObjectModel model;
    model.set_page_range(-1, 2);
    REQUIRE(model.range_start() == 1);
    model.set_page_range(5, 3); // end < start
    REQUIRE(model.range_end() >= model.range_start());
}

TEST_CASE("Page navigation clamped", "[pdf][nav]")
{
    PdfObjectModel model;
    model.set_total_pages(10);
    model.set_current_page(1);
    model.prev_page();
    REQUIRE(model.current_page() == 1); // at boundary
    model.set_current_page(10);
    model.next_page();
    REQUIRE(model.current_page() == 10); // at boundary
}

TEST_CASE("Page navigation sequential", "[pdf][nav]")
{
    PdfObjectModel model;
    model.set_total_pages(5);
    model.set_current_page(3);
    model.next_page();
    REQUIRE(model.current_page() == 4);
    model.prev_page();
    REQUIRE(model.current_page() == 3);
}

TEST_CASE("Render DPI clamped 72-600", "[pdf][quality]")
{
    PdfObjectModel model;
    model.set_render_dpi(50);
    REQUIRE(model.render_dpi() == 72);
    model.set_render_dpi(1000);
    REQUIRE(model.render_dpi() == 600);
}

TEST_CASE("Relink recovers from broken", "[pdf][link]")
{
    PdfObjectModel model;
    model.set_link_state(PdfLinkState::kBroken);
    REQUIRE(model.link_state() == PdfLinkState::kBroken);
    model.relink("/new/path/doc.pdf");
    REQUIRE(model.link_state() == PdfLinkState::kLinked);
    REQUIRE(model.source_path() == "/new/path/doc.pdf");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
