// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ExportModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Export settings format and scope", "[export][settings]")
{
    ExportModel model;
    model.set_settings({ExportFormat::kSvg, ExportScope::kSelection, 300, 2.0, true});
    REQUIRE(model.settings().format == ExportFormat::kSvg);
    REQUIRE(model.settings().scope == ExportScope::kSelection);
    REQUIRE(model.settings().dpi == 300);
    REQUIRE(model.settings().scale == 2.0);
    REQUIRE(model.settings().transparent_bg);
}

TEST_CASE("DPI clamped to valid range", "[export][dpi]")
{
    ExportModel model;
    model.set_dpi(10);
    REQUIRE(model.settings().dpi == 72);
    model.set_dpi(1000);
    REQUIRE(model.settings().dpi == 600);
}

TEST_CASE("Scale clamped to valid range", "[export][scale]")
{
    ExportModel model;
    model.set_scale(0.01);
    REQUIRE(model.settings().scale == 0.1);
    model.set_scale(10.0);
    REQUIRE(model.settings().scale == 4.0);
}

TEST_CASE("Print pagination page tiles", "[export][pagination]")
{
    ExportModel model;
    model.set_page_tiles({{0, 0, 0, 800, 600}, {1, 800, 0, 800, 600}});
    REQUIRE(model.page_count() == 2);
    REQUIRE(model.page_tiles()[0].page_index == 0);
}

TEST_CASE("Quick snapshot counter", "[export][snapshot]")
{
    ExportModel model;
    REQUIRE(model.snapshot_count() == 0);
    model.trigger_snapshot();
    model.trigger_snapshot();
    REQUIRE(model.snapshot_count() == 2);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
