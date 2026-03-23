/// @file test_v27_p01_icon_audit.cpp
/// @brief V27 Phase 01: Icon audit report, source classification, surface mapping.
#include <catch2/catch_test_macros.hpp>
#include "core/V27IconAuditReport.h"
#include "core/IconInventory.h"
#include "core/IconSemanticMapper.h"
#include "core/IconValidationGate.h"

using namespace markamp::core;

TEST_CASE("V27 P01: Icon source count", "[v27][p01]")
{
    REQUIRE(icon_source_count() == 6);
}

TEST_CASE("V27 P01: Icon source labels", "[v27][p01]")
{
    REQUIRE(std::string(icon_source_label(IconSource::kMui)) == "MUI");
    REQUIRE(std::string(icon_source_label(IconSource::kEmoji)) == "Emoji");
    REQUIRE(std::string(icon_source_label(IconSource::kLucide)) == "Lucide");
}

TEST_CASE("V27 P01: Icon surface count", "[v27][p01]")
{
    REQUIRE(icon_surface_count() == 18);
}

TEST_CASE("V27 P01: Icon surface labels", "[v27][p01]")
{
    REQUIRE(std::string(icon_surface_label(IconSurface::kActivityBar)) == "ActivityBar");
    REQUIRE(std::string(icon_surface_label(IconSurface::kCommandPalette)) == "CommandPalette");
    REQUIRE(std::string(icon_surface_label(IconSurface::kCanvas)) == "Canvas");
}

TEST_CASE("V27 P01: Audit report records and counts", "[v27][p01]")
{
    V27IconAuditReport report;
    REQUIRE(report.total_count() == 0);
    report.record({"test_icon", IconSource::kEmoji, IconSurface::kToolbar, "test.cpp", "mui-test", false});
    REQUIRE(report.total_count() == 1);
    REQUIRE(report.count_by_source(IconSource::kEmoji) == 1);
    REQUIRE(report.count_by_surface(IconSurface::kToolbar) == 1);
    REQUIRE(report.migrated_count() == 0);
    REQUIRE(report.legacy_count() == 1);
}

TEST_CASE("V27 P01: Audit report migration progress", "[v27][p01]")
{
    V27IconAuditReport report;
    report.record({"a", IconSource::kMui, IconSurface::kToolbar, "a.cpp", "", true});
    report.record({"b", IconSource::kEmoji, IconSurface::kToolbar, "b.cpp", "mui-b", false});
    REQUIRE(report.overall_progress_percent() == 50);
}

TEST_CASE("V27 P01: Baseline audit populates entries", "[v27][p01]")
{
    V27IconAuditReport report;
    report.populate_v27_baseline();
    REQUIRE(report.total_count() >= 11);
    REQUIRE(report.count_by_source(IconSource::kEmoji) >= 11);
}

TEST_CASE("V27 P01: IconRole count includes V27 additions", "[v27][p01]")
{
    REQUIRE(icon_role_count() == 18);
}

TEST_CASE("V27 P01: IconInventorySource enum values compile", "[v27][p01]")
{
    [[maybe_unused]] auto s1 = IconInventorySource::kMui;
    [[maybe_unused]] auto s2 = IconInventorySource::kEmoji;
    [[maybe_unused]] auto s3 = IconInventorySource::kLucide;
    [[maybe_unused]] auto s4 = IconInventorySource::kManifest;
    REQUIRE(true);
}

TEST_CASE("V27 P01: IconPlacement enum values", "[v27][p01]")
{
    REQUIRE(icon_placement_count() == 5);
    [[maybe_unused]] auto p1 = IconPlacement::kLeading;
    [[maybe_unused]] auto p2 = IconPlacement::kStandalone;
    [[maybe_unused]] auto p3 = IconPlacement::kBadge;
    REQUIRE(true);
}

TEST_CASE("V27 P01: V27BannedPattern struct fields", "[v27][p01]")
{
    V27BannedPattern p{"📄", "emoji", "mui-file-text", "Emoji used as file icon"};
    REQUIRE(p.category == "emoji");
    REQUIRE(p.replacement == "mui-file-text");
}
